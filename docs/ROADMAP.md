# Roadmap and design notes

Each phase ends with a test that has a known right answer. Don't move on until it passes.

## Phase 1: Foundations (done)
- Card encoding: `rank * 4 + suit`, 0..51.
- Reference evaluator: brute-forces the best 5 of n cards. Slow, but obviously correct.
  Verified by the exact category counts over all 2,598,960 five-card hands.
- Vanilla CFR on Kuhn poker. Verified: P1 value converges to −1/18, exploitability → 0,
  and the strategy matches the known equilibrium family (bet K = 3 × bet J, etc.).

## Phase 2: Better CFR on a bigger toy game
- Implement Leduc hold'em (6-card deck, 2 betting rounds). Small enough to solve exactly.
- Add CFR+ and Discounted CFR. Compare exploitability per iteration against vanilla CFR
  on the same game: this is the source of a real "converges Nx faster" number.
- Generalize the solver over a `Game` interface so Kuhn, Leduc, and NLHE share it.

## Phase 3: Speed
- Lookup-table 7-card evaluator. Test it against the reference evaluator on millions of
  random hands, then benchmark hands/sec.
- Equity calculator (Monte Carlo and exact enumeration).

## Phase 4: Heads-up no-limit hold'em
- Game engine with stacks, blinds, legal actions, showdown.
- Action abstraction: fold, call, a few bet sizes (e.g. 0.5x, 1x pot, all-in).
- Card abstraction: bucket hands by k-means over equity distributions.
- Blueprint solve with external-sampling MCCFR. Multithread the iterations.
- Opponent model: track VPIP / PFR / aggression and shift toward exploitative play.

## Phase 5: Measurement
- Self-play harness: play version N vs N-1, report bb/100 (or mbb/hand) with 95% CIs.
- Slumbot client in `scripts/`. Play tens of thousands of hands; poker variance is large.
  Report the result honestly, including a loss. Losing narrowly to Slumbot is a good result.
- Exploitability estimate with local best response (LBR).

## Phase 6 (stretch)
- Real-time depth-limited subgame re-solving.
- Multiway play with rule-based adjustments on top of the blueprint.
- Web UI to play against the bot.
