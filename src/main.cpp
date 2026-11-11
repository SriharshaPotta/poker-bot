// pokerbot CLI
//
//   pokerbot kuhn [iterations]     Train CFR on Kuhn poker and report convergence
//   pokerbot eval <cards>          Evaluate a 5-7 card hand, e.g. "AsKsQsJsTs"
//
// More subcommands (selfplay, slumbot) land as the roadmap progresses.

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "poker/cards.hpp"
#include "poker/evaluator.hpp"
#include "poker/games/kuhn.hpp"

namespace {

int usage() {
  std::cerr << "usage:\n"
            << "  pokerbot kuhn [iterations]\n"
            << "  pokerbot eval <cards>\n";
  return 1;
}

int run_kuhn(int iterations) {
  poker::kuhn::CfrSolver solver;
  std::cout << std::fixed << std::setprecision(6);
  std::cout << "iterations   ev(P1)       target       exploitability\n";
  int done = 0;
  for (int checkpoint = 10; done < iterations; checkpoint *= 10) {
    const int step = std::min(checkpoint, iterations) - done;
    solver.train(step);
    done += step;
    std::cout << std::setw(10) << done << "   " << std::setw(9) << solver.expected_value_p1() << "   "
              << std::setw(9) << poker::kuhn::kGameValueP1 << "   " << solver.exploitability() << "\n";
  }

  std::cout << "\nAverage strategy, P(bet):\n";
  for (const char* key : {"J", "Q", "K", "Jpb", "Qpb", "Kpb", "Jp", "Qp", "Kp", "Jb", "Qb", "Kb"})
    std::cout << "  " << std::setw(4) << key << "  " << solver.average_strategy(key)[1] << "\n";
  return 0;
}

int run_eval(const std::string& text) {
  const auto cards = poker::parse_cards(text);
  const auto value = poker::evaluate(cards);
  std::cout << poker::to_string(cards) << " -> " << poker::to_string(poker::category_of(value))
            << " (value " << value << ")\n";
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 2) return usage();
  const std::string cmd = argv[1];
  try {
    if (cmd == "kuhn") return run_kuhn(argc > 2 ? std::atoi(argv[2]) : 100000);
    if (cmd == "eval" && argc > 2) return run_eval(argv[2]);
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  return usage();
}
