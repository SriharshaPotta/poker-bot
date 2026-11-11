// Milestone 1: CFR must converge to the known Kuhn poker Nash equilibrium.

#include "check.hpp"
#include "poker/games/kuhn.hpp"

using namespace poker::kuhn;

int main() {
  CfrSolver solver;
  solver.train(100000);

  // Game value for player 1 is exactly -1/18.
  CHECK_NEAR(solver.expected_value_p1(), kGameValueP1, 1e-3);

  // Average strategy should be (very close to) unexploitable.
  CHECK(solver.exploitability() < 1e-3);

  // Known structure of every Kuhn equilibrium:
  //   P1 never bets Q first; P2 always bets K after a check and always calls with K;
  //   P2 never calls a bet with J; P1 always calls with K and folds J to a bet.
  CHECK_NEAR(solver.average_strategy("Q")[1], 0.0, 0.02);
  CHECK_NEAR(solver.average_strategy("Kp")[1], 1.0, 0.02);
  CHECK_NEAR(solver.average_strategy("Kb")[1], 1.0, 0.02);
  CHECK_NEAR(solver.average_strategy("Jb")[1], 0.0, 0.02);
  CHECK_NEAR(solver.average_strategy("Kpb")[1], 1.0, 0.02);
  CHECK_NEAR(solver.average_strategy("Jpb")[1], 0.0, 0.02);

  // P1's family of equilibria: bet J with alpha in [0, 1/3], bet K with 3*alpha.
  const double alpha = solver.average_strategy("J")[1];
  CHECK(alpha >= -0.02 && alpha <= 1.0 / 3.0 + 0.02);
  CHECK_NEAR(solver.average_strategy("K")[1], 3 * alpha, 0.05);

  // P2 bets J after a check 1/3 of the time and calls with Q 1/3 of the time.
  CHECK_NEAR(solver.average_strategy("Jp")[1], 1.0 / 3.0, 0.03);
  CHECK_NEAR(solver.average_strategy("Qb")[1], 1.0 / 3.0, 0.03);

  std::cout << "test_kuhn_cfr: ok (ev " << solver.expected_value_p1() << ", exploitability "
            << solver.exploitability() << ")\n";
  return 0;
}
