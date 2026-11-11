#pragma once

// Minimal test helper: no external dependencies, works with CTest.

#include <cmath>
#include <cstdlib>
#include <iostream>

#define CHECK(cond)                                                              \
  do {                                                                           \
    if (!(cond)) {                                                               \
      std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond "\n"; \
      std::exit(1);                                                              \
    }                                                                            \
  } while (0)

#define CHECK_NEAR(a, b, tol)                                                         \
  do {                                                                                \
    const double _a = (a), _b = (b);                                                  \
    if (std::fabs(_a - _b) > (tol)) {                                                 \
      std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK_NEAR failed: " #a " = " << _a \
                << ", " #b " = " << _b << ", tol " << (tol) << "\n";                  \
      std::exit(1);                                                                   \
    }                                                                                 \
  } while (0)
