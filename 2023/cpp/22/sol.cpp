#include <algorithm>
#include <cstdio>
#include <iostream>
#include <queue>
#include <ranges>
#include <tuple>
#include <unordered_set>
#include <vector>

using namespace std;

struct Segment {
  int beg, end;

  int length() const { return end - beg + 1; }
  bool is_empty() const { return length() <= 0; }

  static Segment get_intersection(const Segment &s1, const Segment &s2) {
    return Segment(max(s1.beg, s2.beg), min(s1.end, s2.end));
  }

  static bool intersects(const Segment &s1, const Segment &s2) {
    return !get_intersection(s1, s2).is_empty();
  }
};

ostream &operator<<(ostream &os, const Segment &seg) {
  os << "(" << seg.beg << ", " << seg.end << ")";
  return os;
}

struct Cube {
  Segment x, y, z;
  size_t ind;

  static bool intersects_x_y(const Cube &c1, const Cube &c2) {
    return Segment::intersects(c1.x, c2.x) && Segment::intersects(c1.y, c2.y);
  }

  int top() const { return z.end; }
  int height() const { return z.length(); }
};

ostream &operator<<(ostream &os, const Cube &cb) {
  os << cb.ind << ": "
     << "x=" << cb.x << ", y=" << cb.y << ", z=" << cb.z;
  return os;
}

vector<string> split(const string &s, char c) {
  vector<string> results;

  size_t start_ind = 0;
  while (start_ind < s.size()) {
    size_t end_ind = s.find(c, start_ind);
    if (end_ind == string::npos) {
      end_ind = s.size();
    }
    results.push_back(s.substr(start_ind, end_ind - start_ind));
    start_ind = end_ind + 1;
  }

  return results;
}

vector<Cube> read_input() {
  vector<Cube> cubes;
  string line;

  while (getline(cin, line)) {
    if (line.empty())
      continue;

    auto begins_ends = split(line, '~');

    auto beg_coords = split(begins_ends[0], ',');
    auto end_coords = split(begins_ends[1], ',');

    cubes.emplace_back(Segment{stoi(beg_coords[0]), stoi(end_coords[0])},
                       Segment{stoi(beg_coords[1]), stoi(end_coords[1])},
                       Segment{stoi(beg_coords[2]), stoi(end_coords[2])},
                       cubes.size());
  }

  return cubes;
}

struct CubeGraph {
  vector<Cube> cubes;
  vector<vector<size_t>> over_neighs, under_neighs;
};

ostream &operator<<(ostream &os, const CubeGraph &graph) {
  for (size_t i = 0; i < graph.cubes.size(); ++i) {
    os << graph.cubes[i];
    os << "\tunder: ";
    for (size_t j : graph.under_neighs[i]) {
      cout << graph.cubes[j].ind << ", ";
    }
    os << "\tover: ";
    for (size_t j : graph.over_neighs[i]) {
      cout << graph.cubes[j].ind << ", ";
    }
    cout << endl;
  }
  return os;
}

CubeGraph simulate_fall(const vector<Cube> &cubes) {

  CubeGraph graph;

  graph.cubes = cubes;
  ranges::sort(graph.cubes, [](const Cube &c1, const Cube &c2) {
    return c1.z.beg < c2.z.beg;
  });

  graph.over_neighs.resize(graph.cubes.size());
  graph.under_neighs.resize(graph.cubes.size());

  // should use 2d segment tree, but the input is very small
  for (size_t i = 1; i < graph.cubes.size(); ++i) {
    Cube &curr = graph.cubes[i];
    int max_below_height = 0;
    vector<size_t> under_inds;

    for (size_t j = 0; j < i; ++j) {
      Cube &prev = graph.cubes[j];
      if (Cube::intersects_x_y(prev, curr)) {
        if (prev.top() > max_below_height) {
          max_below_height = prev.top();
          under_inds.clear();
          under_inds.push_back(j);
        } else if (prev.top() == max_below_height) {
          under_inds.push_back(j);
        }
      }
    }

    for (size_t j : under_inds) {
      graph.over_neighs[j].push_back(i);
    }

    graph.under_neighs[i] = std::move(under_inds);
    const int height = curr.height();
    curr.z.beg = max_below_height + 1;
    curr.z.end = curr.z.beg + height - 1;
  }

  return graph;
};

int part_1(const CubeGraph &graph) {
  int num_safe = 0;

  for (size_t i = 0; i < graph.cubes.size(); ++i) {
    if (graph.over_neighs[i].empty()) {
      ++num_safe;
    } else {
      bool all_safe = true;
      for (size_t j : graph.over_neighs[i]) {
        all_safe = all_safe && (graph.under_neighs[j].size() > 1);
      }
      if (all_safe) {
        ++num_safe;
      }
    }
  }
  return num_safe;
}

int remove_and_check_fallen(const CubeGraph &graph, size_t start_ind) {

  CubeGraph fg = graph;
  unordered_set<size_t> fallen;
  queue<size_t> que;

  que.push(start_ind);
  while (!que.empty()) {
    size_t ind = que.front();
    que.pop();
    fallen.insert(ind);

    for (size_t o_ind : fg.over_neighs[ind]) {
      auto &under_vec = fg.under_neighs[o_ind];
      // remove ind from under neighs of o_ind
      under_vec.erase(remove(under_vec.begin(), under_vec.end(), ind),
                      under_vec.end());

      // o_ind has no support
      if (under_vec.empty()) {
        que.push(o_ind);
      }
    }
  }

  return fallen.size() - 1;
}

int part_2(const CubeGraph &graph) {
  int res = 0;
  for (size_t ind = 0; ind < graph.cubes.size(); ind++) {
    res += remove_and_check_fallen(graph, ind);
  }

  return res;
}

int main() {

  auto cubes = read_input();
  auto graph = simulate_fall(cubes);
  // cout << graph << endl;
  cout << "Part 1: " << part_1(graph) << endl;
  cout << "Part 2: " << part_2(graph) << endl;

  return 0;
}
