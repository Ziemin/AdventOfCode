#include <array>
#include <functional>
#include <iostream>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

vector<string> read_input() {
  string line;
  vector<string> maze;
  while (getline(cin, line)) {
    if (!line.empty())
      maze.push_back(std::move(line));
  }

  return maze;
}

unordered_map<char, vector<tuple<int, int>>> adj = {
    {'|', {make_tuple(-1, 0), {1, 0}}},
    {'-', {make_tuple(0, -1), {0, 1}}},
    {'L', {make_tuple(-1, 0), {0, 1}}},
    {'J', {make_tuple(0, -1), {-1, 0}}},
    {'F', {make_tuple(1, 0), {0, 1}}},
    {'7', {make_tuple(1, 0), {0, -1}}},
    {'S', {make_tuple(-1, 0), {0, 1}, {1, 0}, {0, -1}}}};

int64_t part_1(const vector<string> &maze) {

  int pos_i, pos_j;
  for (int i = 0; i < maze.size(); i++) {
    for (int j = 0; j < maze[i].size(); j++) {
      if (maze[i][j] == 'S') {
        pos_i = i;
        pos_j = j;
        break;
      }
    }
  }

  vector<vector<int64_t>> dist(maze.size(),
                               vector<int64_t>(maze[0].size(), -1));

  queue<tuple<int, int>> q;
  int64_t max_dist = 0;
  q.push({pos_i, pos_j});
  dist[pos_i][pos_j] = 0;

  while (!q.empty()) {
    auto [i, j] = q.front();
    q.pop();

    for (auto [a_i, a_j] : adj[maze[i][j]]) {
      int n_i = i + a_i;
      int n_j = j + a_j;
      if (n_i < 0 || n_i >= maze.size())
        continue;
      else if (n_j < 0 || n_j >= maze[0].size())
        continue;
      else if (maze[n_i][n_j] == '.')
        continue;

      bool is_connected = false;

      for (auto [b_i, b_j] : adj[maze[n_i][n_j]]) {
        if (n_i + b_i == i && n_j + b_j == j) {
          is_connected = true;
          break;
        }
      }

      if (!is_connected)
        continue;

      if (dist[n_i][n_j] == -1) {
        dist[n_i][n_j] = dist[i][j] + 1;
        max_dist = max(max_dist, dist[n_i][n_j]);
        q.push({n_i, n_j});
      }
    }
  }

  return max_dist;
}

int64_t part_2(const vector<string> &maze) {

  int pos_i, pos_j;
  for (int i = 0; i < maze.size(); i++) {
    for (int j = 0; j < maze[i].size(); j++) {
      if (maze[i][j] == 'S') {
        pos_i = i;
        pos_j = j;
        break;
      }
    }
  }

  vector<vector<int64_t>> dist(maze.size(),
                               vector<int64_t>(maze[0].size(), -1));

  queue<tuple<int, int>> q;
  q.push({pos_i, pos_j});
  dist[pos_i][pos_j] = 0;

  auto are_connected = [&](tuple<int, int> first, tuple<int, int> second) {
    auto [f_i, f_j] = first;
    auto [s_i, s_j] = second;

    if (f_i < 0 || f_i >= maze.size() || f_j < 0 || f_j >= maze[0].size() ||
        s_i < 0 || s_i >= maze.size() || s_j < 0 || s_j >= maze[0].size())
      return false;

    bool first_second = false;
    bool second_first = false;

    for (auto [b_i, b_j] : adj[maze[f_i][f_j]]) {
      if (f_i + b_i == s_i && f_j + b_j == s_j) {
        first_second = true;
        break;
      }
    }

    for (auto [b_i, b_j] : adj[maze[s_i][s_j]]) {
      if (s_i + b_i == f_i && s_j + b_j == f_j) {
        second_first = true;
        break;
      }
    }

    return first_second && second_first;
  };

  // bfs

  while (!q.empty()) {
    auto [i, j] = q.front();
    q.pop();

    for (auto [a_i, a_j] : adj[maze[i][j]]) {
      int n_i = i + a_i;
      int n_j = j + a_j;

      bool is_connected = are_connected({i, j}, {n_i, n_j});
      if (!is_connected)
        continue;

      if (maze[n_i][n_j] == '.')
        continue;

      if (dist[n_i][n_j] == -1) {
        dist[n_i][n_j] = dist[i][j] + 1;
        q.push({n_i, n_j});
      }
    }
  }

  vector<vector<bool>> visited(maze.size() * 2,
                               vector<bool>(maze[0].size() * 2, false));
  int64_t num_enclosed = 0;

  for (int i = 0; i < maze.size(); i++) {
    for (int j = 0; j < maze[0].size(); j++) {
      if (dist[i][j] >= 0) {
        visited[2 * i][2 * j] = true;

        if (are_connected({i, j}, {i + 1, j})) {
          visited[2 * i + 1][2 * j] = true;
        }
        if (are_connected({i, j}, {i, j + 1})) {
          visited[2 * i][2 * j + 1] = true;
        }
      }
    }
  }

  function<tuple<bool, int64_t>(int i, int j)> dfs = [&](int i, int j) {
    visited[i][j] = true;
    int nm = (i % 2 == 0) && (j % 2 == 0) ? 1 : 0;

    bool out = (i == 0 || i == visited.size() - 1 || j == 0 ||
                j == visited[0].size() - 1);

    for (auto [n_i, n_j] : vector<tuple<int, int>>{
             {i + 1, j},
             {i - 1, j},
             {i, j + 1},
             {i, j - 1},
         }) {

      if (n_i < 0 || n_i >= visited.size())
        continue;
      if (n_j < 0 || n_j >= visited[0].size())
        continue;

      if (!visited[n_i][n_j]) {
        auto [n_out, n_nm] = dfs(n_i, n_j);
        nm += n_nm;
        out |= n_out;
      }
    }

    return make_tuple(out, nm);
  };

  for (int i = 0; i < maze.size(); i++) {
    for (int j = 0; j < maze[0].size(); j++) {
      if (!visited[2 * i][2 * j]) {

        auto [reached_outside, num_visited] = dfs(2 * i, 2 * j);

        if (!reached_outside)
          num_enclosed += num_visited;
      }
    }
  }

  return num_enclosed;
}

int main() {

  auto maze = read_input();
  cout << part_1(maze) << endl;
  cout << part_2(maze) << endl;
  return 0;
}
