# poker-bot

A no-limit Texas hold'em bot in C++, built on counterfactual regret minimization (CFR).

The plan is to verify every piece against something with a known answer before trusting it at scale:
CFR is checked against the exact Nash equilibria of Kuhn and Leduc poker, the hand evaluator against
the exact category counts of all 2,598,960 five-card hands, and the full bot against
[Slumbot](https://www.slumbot.com/), a former Annual Computer Poker Competition champion.

## Status

| Milestone | Status |
|---|---|
| Card parsing, deck | ✅ done |
| Reference hand evaluator (5–7 cards), exhaustively tested | ✅ done |
| Vanilla CFR on Kuhn poker, converges to game value −1/18 | ✅ done |
| Leduc hold'em + CFR+ / Discounted CFR | ⬜ next |
| Fast lookup-table evaluator (tested against the reference) | ⬜ |
| Heads-up NLHE game engine + action abstraction | ⬜ |
| Card abstraction (k-means on equity distributions) | ⬜ |
| Blueprint solve (MCCFR, external sampling) | ⬜ |
| Opponent model (VPIP / PFR / aggression) | ⬜ |
| Self-play regression harness with confidence intervals | ⬜ |
| Slumbot benchmark client | ⬜ |
| Multiway play (3–9 players) | ⬜ |

## Build

Requires CMake 3.20+ and a C++20 compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Usage

```bash
# Train CFR on Kuhn poker and watch it converge to the equilibrium
./build/pokerbot kuhn 100000

# Evaluate a 5–7 card hand
./build/pokerbot eval AsKs2s7s9sQdJd
```

Sample Kuhn output:

```
iterations   ev(P1)       target       exploitability
        10   -0.035974   -0.055556   0.091144
       100   -0.054715   -0.055556   0.023301
      1000   -0.055178   -0.055556   0.006505
     10000   -0.055531   -0.055556   0.001486
    100000   -0.055554   -0.055556   0.000633
```

## Layout

```
include/poker/        public headers
  cards.hpp           card encoding and parsing
  evaluator.hpp       hand strength
  games/kuhn.hpp      Kuhn poker + CFR solver
src/                  implementations and the pokerbot CLI
tests/                dependency-free tests run by CTest
scripts/              Python tooling (Slumbot client, analysis)
docs/ROADMAP.md       design notes and next steps
```

## Responsible use

This bot is for research and benchmarking against other bots. Running bots on real-money poker
sites violates their terms of service. Don't.

## License

MIT
