#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace poker {

// A card is encoded as rank * 4 + suit, giving values 0..51.
// Ranks: 0 = '2' ... 12 = 'A'. Suits: 0 = clubs, 1 = diamonds, 2 = hearts, 3 = spades.
using Card = std::uint8_t;

constexpr int kNumRanks = 13;
constexpr int kNumSuits = 4;
constexpr int kDeckSize = 52;

constexpr int rank_of(Card c) { return c / kNumSuits; }
constexpr int suit_of(Card c) { return c % kNumSuits; }
constexpr Card make_card(int rank, int suit) { return static_cast<Card>(rank * kNumSuits + suit); }

// Parse "As", "Td", "2c" etc. Throws std::invalid_argument on bad input.
Card parse_card(std::string_view s);

// Parse a space-free or space-separated list, e.g. "AsKd" or "As Kd 7h".
std::vector<Card> parse_cards(std::string_view s);

std::string to_string(Card c);
std::string to_string(const std::vector<Card>& cards);

// Fresh, ordered 52-card deck.
std::vector<Card> full_deck();

}  // namespace poker
