#include "poker/games/kuhn.hpp"

#include <algorithm>
#include <functional>
#include <vector>

namespace poker::kuhn {

namespace {

constexpr char kCardChars[] = {'J', 'Q', 'K'};

// All 6 ways to deal 2 of the 3 cards (P1 card, P2 card, unused card).
const std::array<std::array<int, 3>, 6> kDeals = {{
    {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0},
}};

bool is_terminal(const std::string& h) {
  return h == "pp" || h == "bp" || h == "bb" || h == "pbp" || h == "pbb";
}

// Payoff to player 1 at a terminal history.
double terminal_value_p1(const std::array<int, 3>& cards, const std::string& h) {
  const double sign = cards[0] > cards[1] ? 1.0 : -1.0;
  if (h == "pp") return sign;          // check-check showdown for the ante
  if (h == "bp") return 1.0;           // P2 folds to a bet
  if (h == "pbp") return -1.0;         // P1 folds to a bet
  return 2.0 * sign;                   // "bb" or "pbb": bet called, showdown
}

std::string info_set_key(const std::array<int, 3>& cards, const std::string& h) {
  const int player = static_cast<int>(h.size() % 2);
  return std::string(1, kCardChars[cards[player]]) + h;
}

using Policy = std::function<std::array<double, kNumActions>(const std::string&)>;

// Expected payoff to player 1 under the given policies, for one deal.
double evaluate(const std::array<int, 3>& cards, const std::string& h, const Policy& policy) {
  if (is_terminal(h)) return terminal_value_p1(cards, h);
  const auto s = policy(info_set_key(cards, h));
  return s[0] * evaluate(cards, h + 'p', policy) + s[1] * evaluate(cards, h + 'b', policy);
}

// Info sets where `player` (0 or 1) acts.
std::vector<std::string> info_sets_of(int player) {
  std::vector<std::string> out;
  for (char c : kCardChars) {
    if (player == 0) {
      out.push_back(std::string(1, c));
      out.push_back(std::string(1, c) + "pb");
    } else {
      out.push_back(std::string(1, c) + "p");
      out.push_back(std::string(1, c) + "b");
    }
  }
  return out;
}

}  // namespace

std::array<double, kNumActions> InfoSetNode::strategy() const {
  std::array<double, kNumActions> s{};
  double total = 0;
  for (int a = 0; a < kNumActions; ++a) total += s[a] = std::max(regret_sum[a], 0.0);
  for (int a = 0; a < kNumActions; ++a) s[a] = total > 0 ? s[a] / total : 1.0 / kNumActions;
  return s;
}

std::array<double, kNumActions> InfoSetNode::average_strategy() const {
  std::array<double, kNumActions> s{};
  const double total = strategy_sum[0] + strategy_sum[1];
  for (int a = 0; a < kNumActions; ++a) s[a] = total > 0 ? strategy_sum[a] / total : 1.0 / kNumActions;
  return s;
}

// Vanilla CFR. Returns the expected value for player 1.
// p0, p1 are each player's probability of reaching this history.
double CfrSolver::cfr(const std::array<int, 3>& cards, const std::string& h, double p0, double p1) {
  if (is_terminal(h)) return terminal_value_p1(cards, h);

  const int player = static_cast<int>(h.size() % 2);
  InfoSetNode& node = nodes_[info_set_key(cards, h)];
  const auto s = node.strategy();

  std::array<double, kNumActions> util{};
  double node_util = 0;
  for (int a = 0; a < kNumActions; ++a) {
    const std::string next = h + (a == 0 ? 'p' : 'b');
    util[a] = player == 0 ? cfr(cards, next, p0 * s[a], p1) : cfr(cards, next, p0, p1 * s[a]);
    node_util += s[a] * util[a];
  }

  // Values are from P1's view; flip the sign for P2's regrets.
  const double sign = player == 0 ? 1.0 : -1.0;
  const double opp_reach = player == 0 ? p1 : p0;
  const double own_reach = player == 0 ? p0 : p1;
  for (int a = 0; a < kNumActions; ++a) {
    node.regret_sum[a] += opp_reach * sign * (util[a] - node_util);
    node.strategy_sum[a] += own_reach * s[a];
  }
  return node_util;
}

void CfrSolver::train(int iterations) {
  for (int i = 0; i < iterations; ++i)
    for (const auto& deal : kDeals) cfr(deal, "", 1.0, 1.0);
}

std::array<double, kNumActions> CfrSolver::average_strategy(const std::string& info_set) const {
  const auto it = nodes_.find(info_set);
  if (it == nodes_.end()) return {0.5, 0.5};
  return it->second.average_strategy();
}

double CfrSolver::expected_value_p1() const {
  const Policy avg = [this](const std::string& key) { return average_strategy(key); };
  double total = 0;
  for (const auto& deal : kDeals) total += evaluate(deal, "", avg);
  return total / kDeals.size();
}

double CfrSolver::exploitability() const {
  // Each player has 6 info sets with 2 actions, so 2^6 = 64 pure strategies.
  // Brute-force the best response for each side against the average strategy.
  auto best_response_value = [this](int br_player) {
    const auto sets = info_sets_of(br_player);
    double best = -1e9;
    for (int mask = 0; mask < (1 << sets.size()); ++mask) {
      const Policy policy = [&](const std::string& key) -> std::array<double, kNumActions> {
        for (std::size_t i = 0; i < sets.size(); ++i)
          if (sets[i] == key) return (mask >> i) & 1 ? std::array{0.0, 1.0} : std::array{1.0, 0.0};
        return average_strategy(key);
      };
      double total = 0;
      for (const auto& deal : kDeals) total += evaluate(deal, "", policy);
      const double v = (br_player == 0 ? 1.0 : -1.0) * total / kDeals.size();
      best = std::max(best, v);
    }
    return best;
  };
  return (best_response_value(0) + best_response_value(1)) / 2.0;
}

}  // namespace poker::kuhn
