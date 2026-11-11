cmake_minimum_required(VERSION 3.20)
project(poker_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core library: everything except the CLI entry point.
add_library(poker
  src/cards.cpp
  src/evaluator.cpp
  src/games/kuhn.cpp
)
target_include_directories(poker PUBLIC include)

# CLI
add_executable(pokerbot src/main.cpp)
target_link_libraries(pokerbot PRIVATE poker)

# Tests (plain asserts, no external dependencies)
enable_testing()
foreach(t test_cards test_evaluator test_kuhn_cfr)
  add_executable(${t} tests/${t}.cpp)
  target_link_libraries(${t} PRIVATE poker)
  add_test(NAME ${t} COMMAND ${t})
endforeach()
