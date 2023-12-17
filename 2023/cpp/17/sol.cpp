#include <algorithm>
#include <array>
#include <bits/ranges_algo.h>
#include <functional>
#include <iostream>
#include <queue>
#include <ranges>
#include <vector>

using namespace std;

enum class direction : size_t { DOWN, RIGHT, UP, LEFT };

direction turn_left(direction dir) { return (direction)(((int)dir + 1) % 4); }
direction turn_right(direction dir) { return (direction)(((int)dir + 3) % 4); }
size_t dir_to_ind(direction dir) { return (size_t)dir; }

struct pos {
  int64_t y, x;

  pos operator+(pos other) const { return {y + other.y, x + other.x}; }
  pos operator-(pos other) const { return {y - other.y, x - other.x}; }
  pos operator*(int64_t k) const { return {y * k, x * k}; }
  bool operator==(pos other) const { return y == other.y && x == other.x; }
};

pos MOVE[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

struct grid {
  grid(vector<vector<int>> heatmap)
      : Y(heatmap.size()), X(heatmap[0].size()), heatmap(std::move(heatmap)) {}

  const int64_t Y, X;

  tuple<pos, int> get_cost(pos p, direction dir, int steps) const {
    const pos move = MOVE[dir_to_ind(dir)];
    const pos fin_pos = p + (move * steps);

    if (fin_pos == p)
      return {fin_pos, 0};
    if (fin_pos.y < 0 || fin_pos.y >= Y || fin_pos.x < 0 || fin_pos.x >= X)
      return {fin_pos, -1};

    int cost = 0;
    for (pos curr = fin_pos; curr != p; curr = curr - move) {
      cost += heatmap[curr.y][curr.x];
    }
    return {fin_pos, cost};
  }

  vector<vector<int>> heatmap;
};

grid read_grid() {
  vector<vector<int>> heatmap;

  string line;
  while (getline(cin, line)) {
    if (!line.empty()) {
      vector<int> heats;
      for (char c : line)
        heats.push_back(c - '0');
      heatmap.push_back(std::move(heats));
    }
  }

  return grid(std::move(heatmap));
}

using queue_el_t = tuple<int64_t, pos, direction>;

struct PriorityComp {
  bool operator()(const queue_el_t &el1, const queue_el_t &el2) const {
    return greater<int64_t>{}(get<0>(el1), get<0>(el2));
  }
};

int64_t shortest_path(const grid &g, pos start, pos last,
                      const vector<direction> &entry_directions, int steps_min,
                      int steps_max) {

  using last_dim_t = array<int64_t, 4>;

  vector<vector<last_dim_t>> dist(g.Y,
                                  vector<last_dim_t>(g.X, {-1, -1, -1, -1}));

  priority_queue<queue_el_t, vector<queue_el_t>, PriorityComp> que;

  for (auto dir : entry_directions) {
    dist[start.y][start.x][dir_to_ind(dir)] = 0;
    que.push({0, start, dir});
  }

  auto enqueue_nexts = [&](int64_t d, pos p, direction prev_dir,
                           direction next_dir) {
    for (int steps = steps_min; steps <= steps_max; steps++) {
      auto [next_pos, cost] = g.get_cost(p, next_dir, steps);
      if (cost == -1)
        break;
      int64_t next_dist = dist[next_pos.y][next_pos.x][dir_to_ind(next_dir)];
      int64_t new_next_dist = d + cost;

      if (next_dist == -1 || new_next_dist < next_dist) {
        dist[next_pos.y][next_pos.x][dir_to_ind(next_dir)] = new_next_dist;
        que.push({new_next_dist, next_pos, next_dir});
      }
    }
  };

  while (!que.empty()) {
    const auto [d, p, dir] = que.top();
    que.pop();
    if (d != dist[p.y][p.x][dir_to_ind(dir)])
      continue;

    enqueue_nexts(d, p, dir, turn_left(dir));
    enqueue_nexts(d, p, dir, turn_right(dir));
  }

  auto final_els = dist[last.y][last.x] |
                   ranges::views::filter([](auto d) { return d != -1; });
  auto min_it = ranges::min_element(final_els);

  if (min_it == final_els.end())
    return -1;

  return *min_it;
}

int main() {
  grid g = read_grid();
  cout << shortest_path(g, {0, 0}, {g.Y - 1, g.X - 1},
                        {direction::RIGHT, direction::DOWN}, 1, 3)
       << endl;
  cout << shortest_path(g, {0, 0}, {g.Y - 1, g.X - 1},
                        {direction::RIGHT, direction::DOWN}, 4, 10)
       << endl;

  return 0;
}
