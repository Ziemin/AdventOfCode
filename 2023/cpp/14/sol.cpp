#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

int64_t calculate_weight(const vector<string> &dish) {
  int64_t weight = 0;

  for (int r = 0; r < dish.size(); ++r) {
    for (int c = 0; c < dish[r].size(); ++c) {
      if (dish[r][c] == 'O')
        weight += dish.size() - r;
    }
  }

  return weight;
}

enum class direction { NORTH, WEST, EAST, SOUTH };

void tilt_dish(vector<string> &dish, direction dir) {
  size_t R = dish.size(), C = dish[0].size();

  int f_start, f_end, f_step, s_start, s_end, s_step;

  switch (dir) {
  case direction::NORTH:
    f_start = 0;
    f_end = C;
    f_step = 1;
    s_start = 0;
    s_end = R;
    s_step = 1;
    break;
  case direction::SOUTH:
    f_start = 0;
    f_end = C;
    f_step = 1;
    s_start = R - 1;
    s_end = -1;
    s_step = -1;
    break;
  case direction::WEST:
    f_start = 0;
    f_end = R;
    f_step = 1;
    s_start = 0;
    s_end = C;
    s_step = 1;
    break;
  case direction::EAST:
    f_start = 0;
    f_end = R;
    f_step = 1;
    s_start = C - 1;
    s_end = -1;
    s_step = -1;
    break;
  }

  auto get_field = [&](int f, int s) {
    switch (dir) {
    case direction::NORTH:
    case direction::SOUTH:
      return dish[s][f];
    case direction::WEST:
    case direction::EAST:
      return dish[f][s];
    }
  };

  auto set_field = [&](int f, int s, char c) {
    switch (dir) {
    case direction::NORTH:
    case direction::SOUTH:
      dish[s][f] = c;
      return;
    case direction::WEST:
    case direction::EAST:
      dish[f][s] = c;
      return;
    }
  };

  for (int f = f_start; f != f_end; f += f_step) {
    int prev_free = s_start;
    for (int s = s_start; s != s_end; s += s_step) {
      if (get_field(f, s) == 'O') {
        if (prev_free != s) {
          set_field(f, prev_free, 'O');
          set_field(f, s, '.');
        }
        prev_free += s_step;
      } else if (get_field(f, s) == '.') {
        continue;
      } else if (get_field(f, s) == '#') {
        prev_free = s + s_step;
      } else {
        cerr << "f " << f << " s " << s << " dir " << (int)dir << endl;
        throw std::runtime_error(string("Unrecognized object: ") +
                                 get_field(f, s));
      }
    }
  }
}

void run_cycle(vector<string> &dish) {
  tilt_dish(dish, direction::NORTH);
  tilt_dish(dish, direction::WEST);
  tilt_dish(dish, direction::SOUTH);
  tilt_dish(dish, direction::EAST);
}

bool operator==(const vector<string> &v1, const vector<string> &v2) {
  if (v1.size() != v2.size())
    return false;

  for (size_t i = 0; i < v1.size(); ++i)
    if (v1[i] != v2[i])
      return false;

  return true;
}

template <> struct std::hash<vector<string>> {
  size_t operator()(const vector<string> &v) const {

    size_t res = 0;
    for (const auto &s : v)
      res ^= std::hash<string>{}(s);

    return res;
  }
};

int64_t part_1(const vector<string> &dish) {
  vector<string> current = dish;
  tilt_dish(current, direction::NORTH);
  return calculate_weight(current);
}

int64_t part_2(const vector<string> &dish, int64_t cycles) {

  unordered_map<vector<string>, int64_t> visited;
  vector<int64_t> loads;
  vector<string> current = dish;
  auto it = visited.end();
  int64_t ind = 0;
  while (it == visited.end() && ind <= cycles) {
    visited[current] = ind;
    loads.push_back(calculate_weight(current));
    run_cycle(current);
    it = visited.find(current);
    ++ind;
  }

  if (cycles < loads.size()) {
    return loads[cycles];
  }

  int64_t start_ind = it->second;

  int64_t loop_length = loads.size() - start_ind;
  int64_t final_ind = start_ind + (cycles - start_ind) % loop_length;

  return loads[final_ind];
}

vector<string> read_input() {
  vector<string> lines;
  string line;
  while (getline(cin, line)) {
    if (!line.empty())
      lines.push_back(std::move(line));
  }

  return lines;
}

int main() {
  auto dish = read_input();

  cout << part_1(dish) << endl;
  cout << part_2(dish, 1000000000) << endl;

  return 0;
}
