#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

template <typename T> using Vec2d = vector<vector<T>>;

struct Pos {
  int64_t y, x;

  Pos operator+(Pos other) const { return {y + other.y, x + other.x}; }
  Pos operator-(Pos other) const { return {y - other.y, x - other.x}; }
  Pos operator*(int k) const { return {y * k, x * k}; }
  bool operator==(Pos other) const { return y == other.y && x == other.x; }
};

template <> struct std::hash<Pos> {
  size_t operator()(const Pos &p) const {
    auto hs = std::hash<int64_t>{};
    return hs(p.y) ^ hs(p.x);
  }
};

array<Pos, 4> MOVE = {Pos{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

struct Grid {

  vector<string> grid;
  Pos start, end;

  int64_t Y() const { return grid.size(); }
  int64_t X() const { return grid[0].size(); }

  char get(Pos pos) const { return grid[pos.y][pos.x]; }
  bool is_legal(Pos p) const {
    return p.y >= 0 && p.x >= 0 && p.y < Y() && p.x < X() && get(p) != '#';
  }

  bool is_slope(Pos p) const {
    char c = get(p);
    return (c == '<' || c == '>' || c == '^' || c == 'v');
  }

  template <typename T> Vec2d<T> get_2d_array(T init) const {
    return vector<vector<T>>(Y(), vector<T>(X(), init));
  }

  int num_neighs(Pos p) const {
    int num = 0;
    for (const auto mv : MOVE) {
      if (is_legal(p + mv))
        num++;
    }
    return num;
  }
};

Pos slope_to_move(char c) {
  if (c == 'v')
    return MOVE[0];
  if (c == '>')
    return MOVE[1];
  if (c == '^')
    return MOVE[2];
  if (c == '<')
    return MOVE[3];
  throw std::invalid_argument("Bad slope");
}

Grid read_grid() {
  Grid g;
  string line;

  while (getline(cin, line)) {
    if (line.empty())
      continue;
    g.grid.push_back(std::move(line));
  }

  g.start = {0, 1};
  g.end = {g.Y() - 1, g.X() - 2};

  assert(g.grid[0][1] == '.');
  assert(g.grid[g.Y() - 1][g.X() - 2] == '.');

  return g;
}

int dfs(const Grid &grid, Pos pos, Vec2d<bool> &visited) {
  if (pos == grid.end) {
    return 0;
  }

  visited[pos.y][pos.x] = true;

  vector<Pos> next_steps;
  if (grid.is_slope(pos)) {
    Pos next = pos + slope_to_move(grid.get(pos));
    if (!visited[next.y][next.x]) {
      next_steps.push_back(next);
    }
  } else {
    for (const auto &mv : MOVE) {
      Pos next = pos + mv;
      if (grid.is_legal(next) && !visited[next.y][next.x]) {
        next_steps.push_back(next);
      }
    }
  }

  int max_dist = -1;
  for (auto p : next_steps) {
    int sub_dist = dfs(grid, p, visited);
    if (sub_dist >= 0) {
      max_dist = max(max_dist, sub_dist + 1);
    }
  }

  visited[pos.y][pos.x] = false;

  return max_dist;
}

int part_1(const Grid &grid) {
  auto visited = grid.get_2d_array<bool>(false);
  return dfs(grid, grid.start, visited);
}

struct Edge {
  Pos target;
  int dist;
};

using Graph = Vec2d<vector<Edge>>;

void compress_grid(const Grid &grid, Graph &graph, Pos source, Pos current,
                   int dist, Vec2d<bool> &visited) {
  visited[current.y][current.x] = true;

  const int current_neighs = grid.num_neighs(current);
  // path
  if (current_neighs <= 2) {
    // next free steps
    vector<Pos> next_steps;

    for (const auto &mv : MOVE) {
      Pos next = current + mv;
      if (grid.is_legal(next)) {
        // found junction
        if (grid.num_neighs(next) > 2) {
          graph[current.y][current.x].push_back(Edge{next, 1});
          if (!visited[next.y][next.x]) {
            compress_grid(grid, graph, next, next, 0, visited);
          }
        } else if (!visited[next.y][next.x]) {
          next_steps.push_back(next);
        }
      }
    }

    // I'm the end
    if (next_steps.empty()) {
      if (source != current) {
        // add both ends to the graph
        graph[source.y][source.x].push_back(Edge{current, dist});
        graph[current.y][current.x].push_back(Edge{source, dist});
      }
    } else {
      // I'm not the end, pass further
      compress_grid(grid, graph, source, next_steps[0], dist + 1, visited);
    }
  }
  // 2: crossing
  else {

    for (const auto &mv : MOVE) {
      Pos next = current + mv;
      if (grid.is_legal(next)) {
        graph[current.y][current.x].push_back(Edge{next, 1});
        if (!visited[next.y][next.x]) {
          compress_grid(grid, graph, next, next, 0, visited);
        }
      }
    }
  }
}

int dfs_2(const Grid &grid, const Graph &graph, Pos pos, Vec2d<bool> &visited) {
  if (pos == grid.end) {
    return 0;
  }
  visited[pos.y][pos.x] = true;

  int max_dist = -1;
  for (const Edge &e : graph[pos.y][pos.x]) {
    if (visited[e.target.y][e.target.x])
      continue;

    int sub_dist = dfs_2(grid, graph, e.target, visited);
    if (sub_dist >= 0) {
      max_dist = max(max_dist, sub_dist + e.dist);
    }
  }

  visited[pos.y][pos.x] = false;

  return max_dist;
}

int part_2(const Grid &grid) {
  Grid new_grid = grid;
  for (int64_t y = 0; y < new_grid.Y(); ++y) {
    for (int64_t x = 0; x < new_grid.X(); ++x) {
      if (new_grid.is_slope({y, x})) {
        new_grid.grid[y][x] = '.';
      }
    }
  }
  auto visited = grid.get_2d_array<bool>(false);

  Graph graph = grid.get_2d_array<vector<Edge>>({});
  compress_grid(new_grid, graph, new_grid.start, new_grid.start, 0, visited);

  visited = grid.get_2d_array<bool>(false);
  return dfs_2(new_grid, graph, new_grid.start, visited);
}

int main() {
  Grid grid = read_grid();
  cout << part_1(grid) << endl;
  cout << part_2(grid) << endl;

  return 0;
}
