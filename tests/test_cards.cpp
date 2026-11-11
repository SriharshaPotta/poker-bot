#include "check.hpp"
#include "poker/cards.hpp"

#include <set>
#include <stdexcept>

using namespace poker;

int main() {
  CHECK(to_string(parse_card("As")) == "As");
  CHECK(to_string(parse_card("2c")) == "2c");
  CHECK(to_string(parse_card("td")) == "Td");
  CHECK(rank_of(parse_card("Ah")) == 12);
  CHECK(suit_of(parse_card("Ah")) == 2);

  const auto hand = parse_cards("As Kd 7h");
  CHECK(hand.size() == 3);
  CHECK(to_string(hand) == "As Kd 7h");
  CHECK(parse_cards("AsKd7h") == hand);

  bool threw = false;
  try { parse_card("1x"); } catch (const std::invalid_argument&) { threw = true; }
  CHECK(threw);

  const auto deck = full_deck();
  CHECK(deck.size() == 52);
  CHECK(std::set<Card>(deck.begin(), deck.end()).size() == 52);

  std::cout << "test_cards: ok\n";
  return 0;
}
