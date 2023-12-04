#include <algorithm>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

vector<tuple<vector<int>, vector<int>>> read_numbers() {
  string line;
  vector<tuple<vector<int>, vector<int>>> numbers;

  while (getline(cin, line)) {

    string buf;
    istringstream ss(line);
    vector<int> winning, mine;
    vector<int> *target = &winning;

    ss >> buf;
    ss >> buf;

    while (!ss.eof()) {

      ss >> buf;
      if (buf == "|") {
        target = &mine;
      } else {
        target->push_back(std::stoi(buf));
      }
    }

    numbers.emplace_back(std::move(winning), std::move(mine));
  }

  return numbers;
}

uint64_t part_1(const vector<tuple<vector<int>, vector<int>>> &numbers) {
  uint64_t res = 0;
  for (const auto &[winning, mine] : numbers) {

    uint64_t card_res = 0;

    for (int w : winning) {
      for (int m : mine) {
        if (w == m) {

          card_res = card_res == 0 ? 1 : card_res << 1;
          break;
        }
      }
    }

    res += card_res;
  }

  return res;
}

uint64_t part_2(const vector<tuple<vector<int>, vector<int>>> &numbers) {
  uint64_t res = 0;
  vector<int> copies(numbers.size(), 1);

  for (int i = 0; i < numbers.size(); i++) {
    const auto &[winning, mine] = numbers[i];

    int num_matches = 0;

    for (int w : winning) {
      for (int m : mine) {
        if (w == m) {

          num_matches++;
          break;
        }
      }
    }

    for (int j = i + 1; j < min((int)numbers.size(), i + num_matches + 1);
         j++) {
      copies[j] += copies[i];
    }

    res += copies[i];
  }

  return res;
}

int main() {

  auto numbers = read_numbers();

  uint64_t res_1 = part_1(numbers);
  cout << res_1 << endl;
  uint64_t res_2 = part_2(numbers);
  cout << res_2 << endl;

  return 0;
}
