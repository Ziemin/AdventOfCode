#include <algorithm>
#include <cctype>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

vector<string> read_schema() {
  vector<string> schema;

  string line;

  while (getline(cin, line))
    schema.push_back(std::move(line));

  return schema;
}

bool is_digit(const char c) { return isdigit(c); }

bool is_symbol(const char c) { return c != '.' && !is_digit(c); }

bool is_star(const char c) { return c == '*'; }

const vector<tuple<int, int>> offsets = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                                         {0, 1},   {1, -1}, {1, 0},  {1, 1}};

uint64_t part_1(const vector<string> &schema) {

  uint64_t res = 0;
  string buf;
  bool found = false;

  for (size_t l = 0; l < schema.size(); l++) {

    buf.clear();
    found = false;
    size_t k = 0;

    while (k < schema[l].size()) {
      const char c = schema[l][k];

      if (!is_digit(c)) {
        if (found) {
          res += std::stoi(buf);
        }
        found = false;
        buf.clear();
      } else {
        buf.append(1, c);

        for (auto [i, j] : offsets) {
          int y = l + i, x = k + j;
          if (x < 0 || y < 0 || y >= schema.size() || x >= schema[k].size())
            continue;

          found |= is_symbol(schema[y][x]);
        }
      }

      k++;
    }
    if (found) {
      res += std::stoi(buf);
    }
  }
  return res;
}

struct Pos {
  int x, y;
  bool operator==(const Pos &other) const = default;
};

template <> struct std::hash<Pos> {
  size_t operator()(const Pos &p) const noexcept {
    return std::hash<int>{}(p.x) ^ std::hash<int>{}(p.y);
  }
};

uint64_t part_2(const vector<string> &schema) {

  uint64_t res = 0;
  string buf;
  unordered_set<Pos> stars;

  vector<vector<uint64_t>> mults(schema.size(),
                                 vector<uint64_t>(schema[0].size(), 1));
  vector<vector<uint64_t>> counts(schema.size(),
                                  vector<uint64_t>(schema[0].size(), 0));

  for (size_t l = 0; l < schema.size(); l++) {

    buf.clear();
    stars.clear();
    size_t k = 0;

    while (k < schema[l].size()) {
      const char c = schema[l][k];

      if (!is_digit(c)) {
        if (!buf.empty() && !stars.empty()) {
          for (const auto &[x, y] : stars) {
            counts[x][y] += 1;
            mults[x][y] *= std::stoi(buf);
          }
        }
        stars.clear();
        buf.clear();
      } else {
        buf.append(1, c);

        for (auto [i, j] : offsets) {
          int y = l + i, x = k + j;
          if (x < 0 || y < 0 || y >= schema.size() || x >= schema[k].size())
            continue;

          if (is_star(schema[y][x]))
            stars.insert({y, x});
        }
      }

      k++;
    }
    if (!buf.empty() && !stars.empty()) {
      for (const auto &[x, y] : stars) {
        counts[x][y] += 1;
        mults[x][y] *= std::stoi(buf);
      }
    }
  }

  for (size_t l = 0; l < schema.size(); l++) {
    for (size_t k = 0; k < schema[l].size(); k++) {
      if (counts[l][k] == 2)
        res += mults[l][k];
    }
  }

  return res;
}

int main() {

  vector<string> schema = read_schema();

  cout << part_1(schema) << endl;
  cout << part_2(schema) << endl;

  return 0;
}
