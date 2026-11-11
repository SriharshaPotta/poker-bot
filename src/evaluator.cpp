#include "poker/evaluator.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace poker {

namespace {

HandValue pack(HandCategory cat, std::initializer_list<int> ranks) {
  HandValue v = static_cast<HandValue>(cat) << 20;
  int shift = 16;
  for (int r : ranks) {
    v |= static_cast<HandValue>(r) << shift;
    shift -= 4;
  }
  return v;
}

// Returns the high rank of a straight in the 13-bit rank mask, or -1.
int straight_high(unsigned mask) {
  for (int high = 12; high >= 4; --high) {
    const unsigned run = 0x1Fu << (high - 4);
    if ((mask & run) == run) return high;
  }
  // Wheel: A-2-3-4-5
  const unsigned wheel = (1u << 12) | 0xFu;
  if ((mask & wheel) == wheel) return 3;
  return -1;
}

}  // namespace

HandCategory category_of(HandValue v) { return static_cast<HandCategory>(v >> 20); }

const char* to_string(HandCategory c) {
  switch (c) {
    case HandCategory::HighCard: return "High Card";
    case HandCategory::OnePair: return "One Pair";
    case HandCategory::TwoPair: return "Two Pair";
    case HandCategory::Trips: return "Three of a Kind";
    case HandCategory::Straight: return "Straight";
    case HandCategory::Flush: return "Flush";
    case HandCategory::FullHouse: return "Full House";
    case HandCategory::Quads: return "Four of a Kind";
    case HandCategory::StraightFlush: return "Straight Flush";
  }
  return "?";
}

HandValue evaluate5(std::span<const Card, 5> cards) {
  std::array<int, kNumRanks> counts{};
  unsigned mask = 0;
  bool flush = true;
  for (Card c : cards) {
    ++counts[rank_of(c)];
    mask |= 1u << rank_of(c);
    if (suit_of(c) != suit_of(cards[0])) flush = false;
  }

  const int sh = straight_high(mask);
  if (flush && sh >= 0) return pack(HandCategory::StraightFlush, {sh});

  // Group ranks by (count desc, rank desc).
  std::array<std::pair<int, int>, 5> groups{};
  int n = 0;
  for (int r = kNumRanks - 1; r >= 0; --r)
    if (counts[r]) groups[n++] = {counts[r], r};
  std::stable_sort(groups.begin(), groups.begin() + n,
                   [](auto a, auto b) { return a.first > b.first; });

  auto rk = [&](int i) { return groups[i].second; };
  if (groups[0].first == 4) return pack(HandCategory::Quads, {rk(0), rk(1)});
  if (groups[0].first == 3 && groups[1].first == 2) return pack(HandCategory::FullHouse, {rk(0), rk(1)});
  if (flush) return pack(HandCategory::Flush, {rk(0), rk(1), rk(2), rk(3), rk(4)});
  if (sh >= 0) return pack(HandCategory::Straight, {sh});
  if (groups[0].first == 3) return pack(HandCategory::Trips, {rk(0), rk(1), rk(2)});
  if (groups[0].first == 2 && groups[1].first == 2) return pack(HandCategory::TwoPair, {rk(0), rk(1), rk(2)});
  if (groups[0].first == 2) return pack(HandCategory::OnePair, {rk(0), rk(1), rk(2), rk(3)});
  return pack(HandCategory::HighCard, {rk(0), rk(1), rk(2), rk(3), rk(4)});
}

HandValue evaluate(std::span<const Card> cards) {
  const std::size_t n = cards.size();
  if (n < 5 || n > 7) throw std::invalid_argument("evaluate() needs 5 to 7 cards");
  HandValue best = 0;
  std::array<Card, 5> hand{};
  for (std::size_t a = 0; a < n; ++a)
    for (std::size_t b = a + 1; b < n; ++b)
      for (std::size_t c = b + 1; c < n; ++c)
        for (std::size_t d = c + 1; d < n; ++d)
          for (std::size_t e = d + 1; e < n; ++e) {
            hand = {cards[a], cards[b], cards[c], cards[d], cards[e]};
            best = std::max(best, evaluate5(std::span<const Card, 5>(hand)));
          }
  return best;
}

}  // namespace poker
