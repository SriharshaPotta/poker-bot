#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace poker::kuhn {

// Kuhn poker: 3-card deck (J, Q, K), each player antes 1 and gets one card.
// Actions: 'p' (pass/check/fold) and 'b' (bet/call), bet size 1.
// Player 1's value at Nash equilibrium is exactly -1/18.
//
// This is the first correctness milestone for the CFR implementation:
// if CFR here does not converge to -1/18 with near-zero exploitability,
// nothing built on top of it can be trusted.

constexpr int kNumActions = 2;  // 0 = pass, 1 = bet
constexpr double kGameValueP1 = -1.0 / 18.0;

struct InfoSetNode {
  std::array<double, kNumActions> regret_sum{};
  std::array<double, kNumActions> strategy_sum{};

  // Current strategy from regret matching.
  std::array<double, kNumActions> strategy() const;
  // Average strategy over all iterations: this is what converges to equilibrium.
  std::array<double, kNumActions> average_strategy() const;
};

class CfrSolver {
 public:
  // Runs `iterations` of vanilla CFR over all 6 card deals.
  void train(int iterations);

  // Expected value for player 1 when both players use the average strategy.
  double expected_value_p1() const;

  // Exploitability in chips per game: average of each player's best-response
  // gain against the other's average strategy. 0 at exact equilibrium.
  double exploitability() const;

  // Average strategy at an info set like "K" (P1 first action) or "Qpb".
  // Returns {P(pass), P(bet)}. Unvisited info sets return {0.5, 0.5}.
  std::array<double, kNumActions> average_strategy(const std::string& info_set) const;

  const std::unordered_map<std::string, InfoSetNode>& nodes() const { return nodes_; }

 private:
  double cfr(const std::array<int, 3>& cards, const std::string& history, double p0, double p1);

  std::unordered_map<std::string, InfoSetNode> nodes_;
};

}  // namespace poker::kuhn
