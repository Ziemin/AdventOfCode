#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

vector<vector<int64_t>> read_input() {

  vector<vector<int64_t>> values_list;

  vector<int64_t> values;
  string line;
  int64_t next_val;

  while (getline(cin, line)) {
    if (line.empty())
      break;
    stringstream ss(line);
    while (!ss.eof()) {
      ss >> next_val;
      values.push_back(next_val);
    }
    values_list.push_back(std::move(values));
  }

  return values_list;
}

int64_t get_diffs_rec(const vector<int64_t> &values) {

  bool all_zeros = true;

  vector<int64_t> new_diffs;

  for (int i = 1; i < values.size(); i++) {
    int64_t diff = values[i] - values[i - 1];
    new_diffs.push_back(diff);
    all_zeros &= diff == 0;
  }

  if (all_zeros) {
    return *values.rbegin();
  }

  auto extr = get_diffs_rec(new_diffs);
  int64_t new_extr = (*values.rbegin()) + extr;
  return new_extr;
}

int64_t part_1(const vector<vector<int64_t>> lines) {

  int64_t res = 0;

  for (const auto &values : lines) {
    auto val = get_diffs_rec(values);
    res += val;
  }

  return res;
}

int64_t get_diffs_rec_left(const vector<int64_t> &values) {

  bool all_zeros = true;

  vector<int64_t> new_diffs;

  for (int i = 1; i < values.size(); i++) {
    int64_t diff = values[i] - values[i - 1];
    new_diffs.push_back(diff);
    all_zeros &= diff == 0;
  }

  if (all_zeros) {
    return *values.begin();
  }

  auto extr = get_diffs_rec_left(new_diffs);
  int64_t new_extr = (*values.begin()) - extr;
  return new_extr;
}

int64_t part_2(const vector<vector<int64_t>> lines) {

  int64_t res = 0;

  for (const auto &values : lines) {
    auto val = get_diffs_rec_left(values);
    res += val;
  }

  return res;
}

int main() {

  auto lines = read_input();
  cout << part_1(lines) << endl;
  cout << part_2(lines) << endl;

  return 0;
}
