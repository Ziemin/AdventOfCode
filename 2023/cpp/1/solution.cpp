#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ranges>

int main() {

  auto is_digit = [](char c) -> bool { return std::isdigit(c); };
  auto values = std::views::istream<std::string>(std::cin) |
                std::views::transform([&is_digit](auto &s) -> uint64_t {
                  auto fit = std::ranges::find_if(s, is_digit);

                  if (fit != std::end(s)) {
                    auto eit =
                        std::ranges::find_if(s | std::views::reverse, is_digit);
                    return (*fit - '0') * 10 + (*eit - '0');
                  }
                  return 0;
                });

  uint64_t result = 0;
  for (auto s : values)
    result += s;
  std::cout << result << std::endl;

  return 0;
}
