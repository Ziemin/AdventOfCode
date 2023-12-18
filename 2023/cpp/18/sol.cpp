#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

enum class direction : size_t { DOWN, RIGHT, UP, LEFT };
size_t dir_to_ind(direction dir) { return (size_t)dir; }

struct Move {
  direction dir;
  int64_t count;
};

tuple<vector<Move>, vector<Move>> read_input() {

  vector<Move> one_moves;
  vector<Move> two_moves;
  string line;
  while (getline(cin, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);

    Move one_move;
    Move two_move;

    char dir;
    ss >> dir;
    if (dir == 'L')
      one_move.dir = direction::LEFT;
    else if (dir == 'R')
      one_move.dir = direction::RIGHT;
    else if (dir == 'D')
      one_move.dir = direction::DOWN;
    else if (dir == 'U')
      one_move.dir = direction::UP;
    else
      throw std::runtime_error("bad direction");

    ss >> one_move.count;

    string color;
    ss >> color;

    two_move.count = std::stoll(color.substr(2, 5), nullptr, 16);
    if (color[7] == '0')
      two_move.dir = direction::RIGHT;
    else if (color[7] == '1')
      two_move.dir = direction::DOWN;
    else if (color[7] == '2')
      two_move.dir = direction::LEFT;
    else if (color[7] == '3')
      two_move.dir = direction::UP;
    else
      throw std::runtime_error("bad direction");

    one_moves.push_back(std::move(one_move));
    two_moves.push_back(std::move(two_move));
  }

  return {one_moves, two_moves};
}

struct pos {
  int64_t y, x;

  pos operator+(pos other) const { return {y + other.y, x + other.x}; }
  pos operator-(pos other) const { return {y - other.y, x - other.x}; }
  pos operator*(int64_t k) const { return {y * k, x * k}; }
  bool operator==(pos other) const { return y == other.y && x == other.x; }
};

array<pos, 4> MOVE = {pos{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

pos dir_to_move(direction dir) { return MOVE[dir_to_ind(dir)]; }

tuple<pos, pos> get_dimensions_and_start_pos(const vector<Move> &moves) {

  pos current{0, 0}, max_pos{0, 0}, min_pos{0, 0};

  for (const auto &mv : moves) {
    current = current + dir_to_move(mv.dir) * mv.count;
    max_pos.y = max(max_pos.y, current.y);
    max_pos.x = max(max_pos.x, current.x);
    min_pos.y = min(min_pos.y, current.y);
    min_pos.x = min(min_pos.x, current.x);
  }

  pos dim{max_pos.y - min_pos.y + 1, max_pos.x - min_pos.x + 1};
  pos start_pos{-min_pos.y, -min_pos.x};
  return make_tuple(dim, start_pos);
}

int64_t shoelace(const vector<Move> &moves) {
  pos current{0, 0};
  int64_t area = 0;
  for (const auto &mv : moves) {
    pos next = current + dir_to_move(mv.dir) * mv.count;
    area += current.x * next.y - current.y * next.x;
    current = next;
  }
  return area / 2;
}

int64_t perimeter(const vector<Move> &moves) {
  int64_t p = 0;
  for (const auto &[dir, cnt] : moves)
    p += cnt;
  return p;
}

int64_t full_area(const vector<Move> &moves) {
  return shoelace(moves) + perimeter(moves) / 2 + 1;
}

int main() {

  const auto [one_moves, two_moves] = read_input();
  cout << full_area(one_moves) << endl;
  cout << full_area(two_moves) << endl;
  return 0;
}
