#pragma once

#include <cstdint>
#include <span>

#include "poker/cards.hpp"

namespace poker {

enum class HandCategory : int {
  HighCard = 0,
  OnePair,
  TwoPair,
  Trips,
  Straight,
  Flush,
  FullHouse,
  Quads,
  StraightFlush,
};

// Hand strength: higher is better. Two hands tie iff their strengths are equal.
// Layout: category in bits 20..23, then up to five 4-bit rank tiebreakers.
using HandValue = std::uint32_t;

HandCategory category_of(HandValue v);
const char* to_string(HandCategory c);

// Exact strength of a 5-card hand.
HandValue evaluate5(std::span<const Card, 5> cards);

// Best 5-card hand out of 5..7 cards.
//
// Baseline implementation: checks all C(n,5) subsets (21 for 7 cards).
// Correct but slow; the roadmap replaces it with a lookup-table evaluator
// and uses this one as the reference in tests.
HandValue evaluate(std::span<const Card> cards);

}  // namespace poker
