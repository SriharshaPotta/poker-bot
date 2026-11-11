"""Slumbot benchmark client (stub).

Plan:
  1. Read Slumbot's current API docs at https://www.slumbot.com/ (endpoints, auth, action format).
  2. For each hand: start a new hand, translate Slumbot's action string into our game state,
     ask the C++ bot for an action (via a CLI subcommand or a Python binding), send it back.
  3. Log every hand to a CSV, then report win rate in mbb/hand with a 95% confidence interval.

Nothing here talks to the network yet. Check the API docs before writing the requests,
since endpoint details may have changed.
"""

from __future__ import annotations

import math
import statistics


def win_rate_mbb_per_hand(results_bb: list[float]) -> tuple[float, float]:
    """Mean and 95% CI half-width, in milli-big-blinds per hand."""
    if len(results_bb) < 2:
        raise ValueError("need at least 2 hands")
    mean = statistics.fmean(results_bb) * 1000
    stderr = statistics.stdev(results_bb) / math.sqrt(len(results_bb)) * 1000
    return mean, 1.96 * stderr


def main() -> None:
    raise SystemExit("Slumbot client not implemented yet: see docs/ROADMAP.md, Phase 5.")


if __name__ == "__main__":
    main()
