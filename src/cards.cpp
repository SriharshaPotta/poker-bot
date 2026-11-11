#include "poker/cards.hpp"

#include <stdexcept>

namespace poker {

namespace {
constexpr std::string_view kRankChars = "23456789TJQKA";
constexpr std::string_view kSuitChars = "cdhs";
}  // namespace

Card parse_card(std::string_view s) {
  if (s.size() != 2) throw std::invalid_argument("card must be 2 characters: " + std::string(s));
  const auto r = kRankChars.find(static_cast<char>(std::toupper(static_cast<unsigned char>(s[0]))));
  const auto u = kSuitChars.find(static_cast<char>(std::tolower(static_cast<unsigned char>(s[1]))));
  if (r == std::string_view::npos || u == std::string_view::npos)
    throw std::invalid_argument("bad card: " + std::string(s));
  return make_card(static_cast<int>(r), static_cast<int>(u));
}

std::vector<Card> parse_cards(std::string_view s) {
  std::vector<Card> out;
  std::string token;
  for (char ch : s) {
    if (ch == ' ' || ch == ',') continue;
    token.push_back(ch);
    if (token.size() == 2) {
      out.push_back(parse_card(token));
      token.clear();
    }
  }
  if (!token.empty()) throw std::invalid_argument("trailing characters in card list");
  return out;
}

std::string to_string(Card c) {
  return {kRankChars[rank_of(c)], kSuitChars[suit_of(c)]};
}

std::string to_string(const std::vector<Card>& cards) {
  std::string out;
  for (std::size_t i = 0; i < cards.size(); ++i) {
    if (i) out += ' ';
    out += to_string(cards[i]);
  }
  return out;
}

std::vector<Card> full_deck() {
  std::vector<Card> deck(kDeckSize);
  for (int i = 0; i < kDeckSize; ++i) deck[i] = static_cast<Card>(i);
  return deck;
}

}  // namespace poker
