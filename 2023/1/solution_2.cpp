#include <algorithm>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ranges>
#include <tuple>
#include <vector>

int main() {

  const std::vector<std::pair<std::string, uint>> patterns = {
      {"1", 1},     {"2", 2},     {"3", 3},    {"4", 4},    {"5", 5},
      {"6", 6},     {"7", 7},     {"8", 8},    {"9", 9},    {"one", 1},
      {"two", 2},   {"three", 3}, {"four", 4}, {"five", 5}, {"six", 6},
      {"seven", 7}, {"eight", 8}, {"nine", 9},
  };
  auto rev_patterns_rng = patterns | std::views::transform([](auto v) {
                            std::reverse(v.first.begin(), v.first.end());
                            return v;
                          });
  const std::vector rev_patterns(rev_patterns_rng.begin(),
                                 rev_patterns_rng.end());

  auto values =
      std::views::istream<std::string>(std::cin) |
      std::views::transform([&](const auto &s) -> uint64_t {
        std::string rev_s = s;
        std::reverse(rev_s.begin(), rev_s.end());

        auto starts_rng =
            patterns | std::views::transform([&s](const auto &ptrn) {
              return std::make_tuple(s.find(ptrn.first), ptrn.second);
            }) |
            std::views::filter([](const auto &pos) {
              return std::get<0>(pos) != std::string::npos;
            });

        auto min_start = std::ranges::min_element(starts_rng);

        auto ends_rng =
            rev_patterns | std::views::transform([&rev_s](const auto &ptrn) {
              return std::make_tuple(rev_s.find(ptrn.first), ptrn.second);
            }) |
            std::views::filter([](const auto &pos) {
              return std::get<0>(pos) != std::string::npos;
            });

        auto min_end = std::ranges::min_element(ends_rng);

        return std::get<1>(*min_start) * 10 + std::get<1>(*min_end);
      });

  uint64_t result = 0;
  for (auto s : values)
    result += s;
  std::cout << result << std::endl;

  return 0;
}
