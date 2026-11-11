#include "check.hpp"
#include "poker/evaluator.hpp"

#include <array>
#include <map>

using namespace poker;

namespace {
HandValue ev(const char* s) { return evaluate(parse_cards(s)); }
HandCategory cat(const char* s) { return category_of(ev(s)); }
}  // namespace

int main() {
  // Categories
  CHECK(cat("AsKsQsJsTs") == HandCategory::StraightFlush);
  CHECK(cat("9h9d9c9s2d") == HandCategory::Quads);
  CHECK(cat("KhKdKc3s3d") == HandCategory::FullHouse);
  CHECK(cat("Ah9h7h4h2h") == HandCategory::Flush);
  CHECK(cat("9c8d7h6s5c") == HandCategory::Straight);
  CHECK(cat("Ac2d3h4s5c") == HandCategory::Straight);  // wheel
  CHECK(cat("QhQdQc7s2d") == HandCategory::Trips);
  CHECK(cat("JhJd4c4s9d") == HandCategory::TwoPair);
  CHECK(cat("ThTd8c4s2d") == HandCategory::OnePair);
  CHECK(cat("Ah Jd 8c 4s 2d") == HandCategory::HighCard);

  // Ordering and tiebreaks
  CHECK(ev("6c5d4h3s2c") > ev("Ac2d3h4s5c"));      // 6-high straight beats the wheel
  CHECK(ev("AhAdKc7s2d") > ev("AhAdQc7s2d"));      // kicker
  CHECK(ev("KhKd2c2s9d") > ev("QhQdJcJs9d"));      // top pair of two pair decides
  CHECK(ev("Ah9h7h4h3h") > ev("Ah9h7h4h2h"));      // flush, last card decides
  CHECK(ev("AsKsQsJsTs") == ev("AhKhQhJhTh"));     // suits don't matter
  CHECK(ev("2c3d4h5s7c") < ev("2c2d4h5s7c"));      // high card < pair

  // 7-card: best five out of seven
  CHECK(cat("AsKs 2s7s9s Qd Jd") == HandCategory::Flush);
  CHECK(cat("AsKd QhJcTs 2c 2d") == HandCategory::Straight);
  CHECK(cat("AsAd AhKcKs 2c 2d") == HandCategory::FullHouse);

  // Exhaustive check over all C(52,5) = 2,598,960 hands against known category counts.
  std::map<HandCategory, long> counts;
  std::array<Card, 5> h{};
  for (int a = 0; a < 52; ++a)
    for (int b = a + 1; b < 52; ++b)
      for (int c = b + 1; c < 52; ++c)
        for (int d = c + 1; d < 52; ++d)
          for (int e = d + 1; e < 52; ++e) {
            h = {Card(a), Card(b), Card(c), Card(d), Card(e)};
            ++counts[category_of(evaluate5(std::span<const Card, 5>(h)))];
          }
  CHECK(counts[HandCategory::StraightFlush] == 40);
  CHECK(counts[HandCategory::Quads] == 624);
  CHECK(counts[HandCategory::FullHouse] == 3744);
  CHECK(counts[HandCategory::Flush] == 5108);
  CHECK(counts[HandCategory::Straight] == 10200);
  CHECK(counts[HandCategory::Trips] == 54912);
  CHECK(counts[HandCategory::TwoPair] == 123552);
  CHECK(counts[HandCategory::OnePair] == 1098240);
  CHECK(counts[HandCategory::HighCard] == 1302540);

  std::cout << "test_evaluator: ok\n";
  return 0;
}
