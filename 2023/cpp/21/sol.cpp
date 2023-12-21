#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>

using namespace std;

struct pos {
  int64_t y, x;

  pos operator+(pos other) const { return {y + other.y, x + other.x}; }
  pos operator-(pos other) const { return {y - other.y, x - other.x}; }
  pos operator*(int k) const { return {y * k, x * k}; }
  bool operator==(pos other) const { return y == other.y && x == other.x; }
};

array<pos, 4> MOVE = {pos{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

struct Grid {
  vector<vector<bool>> cells;

  int64_t Y() const { return cells.size(); }
  int64_t X() const { return cells[0].size(); }

  bool is_legal(pos p) const {
    return p.y >= 0 && p.x >= 0 && p.y < Y() && p.x < X() && cells[p.y][p.x];
  }
};

tuple<Grid, pos> read_grid() {
  vector<vector<bool>> cells;
  pos start;

  string line;
  while (getline(cin, line)) {
    if (line.empty())
      continue;

    vector<bool> row;
    for (int x = 0; x < line.size(); ++x) {
      const char c = line[x];
      if (c == '.') {
        row.push_back(true);
      } else if (c == '#') {
        row.push_back(false);
      } else if (c == 'S') {
        row.push_back(true);
        start.y = cells.size();
        start.x = x;
      } else {
        throw std::invalid_argument("bad cell value");
      }
    }
    cells.push_back(std::move(row));
  }

  return {Grid{cells}, start};
}

vector<vector<int>> bfs(const Grid &grid, pos start) {
  vector<vector<int>> dist(grid.Y(), vector<int>(grid.X(), -1));
  queue<pos> que;

  dist[start.y][start.x] = 0;
  que.push(start);
  while (!que.empty()) {
    pos p = que.front();
    que.pop();

    for (const auto &mv : MOVE) {
      pos n_p = p + mv;
      if (grid.is_legal(n_p) && dist[n_p.y][n_p.x] == -1) {
        dist[n_p.y][n_p.x] = dist[p.y][p.x] + 1;
        que.push(n_p);
      }
    }
  }

  return dist;
}

int get_num_reachable_single(const Grid &grid, pos start, int steps) {
  auto dist = bfs(grid, start);
  int count = 0;
  for (int y = 0; y < grid.Y(); ++y) {
    for (int x = 0; x < grid.X(); ++x) {
      if (dist[y][x] != -1 && dist[y][x] <= steps &&
          dist[y][x] % 2 == steps % 2) {
        count++;
      }
    }
  }

  return count;
}

Grid repeat_grid(const Grid &grid, int k) {
  Grid rep_grid = grid;
  // copy in X
  for (int y = 0; y < grid.Y(); ++y) {
    for (int i = 0; i < 2 * k; ++i) {
      copy(grid.cells[y].begin(), grid.cells[y].end(),
           back_inserter(rep_grid.cells[y]));
    }
    assert(rep_grid.cells[y].size() == grid.X() * (2 * k + 1));
  }
  // copy in Y, reserve earlier not to break iterators
  rep_grid.cells.reserve(grid.Y() * (2 * k + 1));
  for (int i = 0; i < 2 * k; ++i) {
    copy(rep_grid.cells.begin(), rep_grid.cells.begin() + grid.Y(),
         back_inserter(rep_grid.cells));
  }
  assert(rep_grid.cells.size() == grid.Y() * (2 * k + 1));

  return rep_grid;
}

void print_pattern_reachability(const Grid &grid, pos start, int k,
                                int radius = -2) {
  pos new_start{grid.Y() * k + start.y, grid.X() * k + start.x};
  Grid rep_grid = repeat_grid(grid, k);

  auto dist = bfs(rep_grid, new_start);
  for (int y = 0; y < rep_grid.Y(); ++y) {
    if (y != 0 && y % grid.Y() == 0)
      cout << endl;

    for (int x = 0; x < rep_grid.X(); ++x) {
      if (x != 0 && x % grid.X() == 0)
        cout << ' ';

      if (dist[y][x] == -1)
        cout << '#';
      else if (dist[y][x] % 2 == 1)
        cout << '1';
      else
        cout << '.';
    }
    cout << endl;
  }
  cout << endl;

  for (int y = 0; y < rep_grid.Y(); ++y) {
    if (y != 0 && y % grid.Y() == 0)
      cout << endl;

    for (int x = 0; x < rep_grid.X(); ++x) {
      if (x != 0 && x % grid.X() == 0)
        cout << "  ";
      if (dist[y][x] == -1)
        cout << "## ";
      else if (dist[y][x] == radius) {
        cout << "\033[1;31m%% \033[0m";
      } else {
        cout << dist[y][x] << " ";
        if (dist[y][x] < 10)
          cout << " ";
      }
    }
    cout << endl;
  }

  cout << endl << " diffs " << endl;
  for (int y = 0; y < rep_grid.Y(); ++y) {
    if (y != 0 && y % grid.Y() == 0)
      cout << endl;

    for (int x = 0; x < rep_grid.X(); ++x) {

      if (x != 0 && x % grid.X() == 0)
        cout << "  ";
      if (dist[y][x] == -1) {
        cout << "## ";
      } else {

        int c = x / grid.X();
        int r = y / grid.Y();
        int prev_c = c;
        int prev_r = r;
        if (abs(c - k) > abs(r - k)) {
          if (c < k)
            prev_c = c + 1;
          else if (c > k)
            prev_c = c - 1;
        } else {
          if (r < k)
            prev_r = r + 1;
          else if (r > k)
            prev_r = r - 1;
        }

        int64_t d = dist[y][x] - dist[prev_r * grid.Y() + y % grid.Y()]
                                     [prev_c * grid.X() + x % grid.X()];
        d = abs(d);
        cout << d << " ";
        if (d < 10)
          cout << " ";
      }
    }
    cout << endl;
  }
}

int64_t get_num_repeats(const Grid &grid, pos start) {
  // assuming grid is a square, we're expanding it until all diffs are euqal to
  // its length
  const int64_t target_diff = grid.Y();
  const int64_t Y = grid.Y();
  const int64_t X = grid.X();

  int k = 1;

  while (true) {
    pos new_start{Y * k + start.y, X * k + start.x};
    auto rep_grid = repeat_grid(grid, k);
    auto dist = bfs(rep_grid, new_start);

    bool all_diff = true;
    for (int r = 0; r < 2 * k + 1; ++r) {
      for (int c = 0; c < 2 * k + 1; ++c) {

        if (r == 0 || r == 2 * k || c == 0 || c == 2 * k) {
          int prev_c = c;
          int prev_r = r;
          if (abs(c - k) > abs(r - k)) {
            if (c < k)
              prev_c = c + 1;
            else if (c > k)
              prev_c = c - 1;
          } else {
            if (r < k)
              prev_r = r + 1;
            else if (r > k)
              prev_r = r - 1;
          }

          for (int y = 0; y < Y; ++y) {
            for (int x = 0; x < X; ++x) {

              int64_t d = dist[r * Y + y][r * X + x] -
                          dist[prev_r * Y + y][prev_c * X + x];
              if (k == 4) {
                cout << "k = " << k << " " << c << " " << r << " " << d << endl;
              }
              all_diff = all_diff && (d == target_diff);
            }
          }
        }
      }
    }

    if (all_diff)
      return k;
    ++k;
    cout << "k = " << k << endl;
  }
}

int64_t get_num_reachable_infinite(const Grid &grid, pos start, int64_t steps) {
  assert(grid.X() == grid.X());
  auto dist = bfs(grid, start);
  // assuming square
  const int64_t width = grid.X();
  const int64_t radius = width / 2;

  int64_t num_odd = 0, num_even = 0;
  int64_t num_odd_cuts = 0, num_even_cuts = 0;

  for (int y = 0; y < width; ++y) {
    for (int x = 0; x < width; ++x) {
      int64_t d = dist[y][x];
      if (d != -1) {
        num_odd += d % 2;
        num_even += (d + 1) % 2;
        if (d > radius) {
          num_odd_cuts += d % 2;
          num_even_cuts += (d + 1) % 2;
        }
      }
    }
  }
  // this turns out to be a nice number for the problem input, 202300
  const int64_t n = (steps - radius) / width;
  const int64_t even_blocks = n * n;
  const int64_t odd_blocks = (n + 1) * (n + 1);

  return odd_blocks * num_odd + even_blocks * num_even -
         (n + 1) * num_odd_cuts + n * num_even_cuts - n;
}

int main() {
  auto [grid, start] = read_grid();
  cout << "single 1 " << get_num_reachable_single(grid, start, 1) << endl;
  cout << "single 6 " << get_num_reachable_single(grid, start, 6) << endl;
  cout << "single 63 " << get_num_reachable_single(grid, start, 63) << endl;
  cout << "single 64 " << get_num_reachable_single(grid, start, 64) << endl;
  // cout << "0" << endl;
  // print_pattern_reachability(grid, start, 0);
  // cout << "1" << endl;
  // print_pattern_reachability(grid, start, 1);
  // cout << "2" << endl;
  // print_pattern_reachability(grid, start, 2);
  cout << get_num_reachable_infinite(grid, start, 26501365) << endl;

  return 0;
}
