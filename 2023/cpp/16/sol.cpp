#include <algorithm>
#include <any>
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <queue>
#include <ranges>
#include <unordered_map>
#include <vector>

using namespace std;

vector<string> read_input() {
  vector<string> grid;

  for (string s; getline(cin, s);) {
    if (!s.empty())
      grid.push_back(std::move(s));
  }

  return grid;
}

enum class direction : size_t { UP = 0, DOWN, RIGHT, LEFT };

size_t dir_to_ind(direction dir) { return (size_t)dir; }

tuple<int, int> move_dir(int y, int x, direction dir) {
  switch (dir) {
  case direction::UP:
    return {y - 1, x};
  case direction::DOWN:
    return {y + 1, x};
  case direction::RIGHT:
    return {y, x + 1};
  case direction::LEFT:
    return {y, x - 1};
  }
}
template <> struct std::hash<direction> {

  size_t operator()(direction dir) const {
    return std::hash<size_t>{}(dir_to_ind(dir) + 1);
  }
};

template <> struct std::hash<tuple<char, direction>> {
  size_t operator()(const tuple<char, direction> &p) const {
    return std::hash<char>{}(get<0>(p)) ^ std::hash<direction>{}(get<1>(p));
  }
};

const unordered_map<tuple<char, direction>, vector<direction>> DIR_CHANGE = {
    {{'.', direction::DOWN}, {direction::DOWN}},
    {{'.', direction::UP}, {direction::UP}},
    {{'.', direction::LEFT}, {direction::LEFT}},
    {{'.', direction::RIGHT}, {direction::RIGHT}},
    {{'/', direction::DOWN}, {direction::LEFT}},
    {{'/', direction::UP}, {direction::RIGHT}},
    {{'/', direction::LEFT}, {direction::DOWN}},
    {{'/', direction::RIGHT}, {direction::UP}},
    {{'\\', direction::DOWN}, {direction::RIGHT}},
    {{'\\', direction::UP}, {direction::LEFT}},
    {{'\\', direction::LEFT}, {direction::UP}},
    {{'\\', direction::RIGHT}, {direction::DOWN}},
    {{'-', direction::DOWN}, {direction::LEFT, direction::RIGHT}},
    {{'-', direction::UP}, {direction::LEFT, direction::RIGHT}},
    {{'-', direction::RIGHT}, {direction::RIGHT}},
    {{'-', direction::LEFT}, {direction::LEFT}},
    {{'|', direction::DOWN}, {direction::DOWN}},
    {{'|', direction::UP}, {direction::UP}},
    {{'|', direction::LEFT}, {direction::UP, direction::DOWN}},
    {{'|', direction::RIGHT}, {direction::UP, direction::DOWN}}};

int64_t part_1(const vector<string> &grid, int start_y, int start_x,
               direction start_dir) {

  const size_t N = grid.size(), M = grid[0].size();

  queue<tuple<int, int, direction>> que;
  vector<vector<array<bool, 4>>> visited(
      N, vector<array<bool, 4>>(M, {false, false, false, false}));

  que.push({start_y, start_x, start_dir});
  visited[start_y][start_x][dir_to_ind(start_dir)] = true;

  while (!que.empty()) {
    const auto [y, x, dir] = que.front();
    que.pop();

    vector<tuple<int, int, direction>> nexts;
    const char cell = grid[y][x];

    for (const auto next_dir : DIR_CHANGE.at({cell, dir})) {
      auto [n_y, n_x] = move_dir(y, x, next_dir);
      if (n_x >= 0 && n_x < M && n_y >= 0 && n_y < N &&
          !visited[n_y][n_x][dir_to_ind(next_dir)]) {
        que.push({n_y, n_x, next_dir});
        visited[n_y][n_x][dir_to_ind(next_dir)] = true;
      }
    }
  }

  int64_t result = 0;
  for (const auto &line : visited) {
    for (const auto &cell : line) {
      if (ranges::any_of(cell, identity{}))
        result++;
    }
  }

  return result;
}

int64_t part_2(const vector<string> &grid) {
  int64_t best = 0;

  for (size_t y = 0; y < grid.size(); ++y) {
    best = max(best, part_1(grid, y, 0, direction::RIGHT));
    best = max(best, part_1(grid, y, grid[y].size() - 1, direction::LEFT));
  }
  for (size_t x = 0; x < grid[0].size(); ++x) {
    best = max(best, part_1(grid, 0, x, direction::DOWN));
    best = max(best, part_1(grid, grid.size() - 1, x, direction::UP));
  }

  return best;
}

int main() {
  auto grid = read_input();
  cout << part_1(grid, 0, 0, direction::RIGHT) << endl;
  cout << part_2(grid) << endl;
  return 0;
}
