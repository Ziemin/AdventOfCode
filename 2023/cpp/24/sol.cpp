#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <z3++.h>

using namespace std;

struct P3 {
  // or a, b, c
  __int128 x, y, z;
};

P3 operator-(const P3 &v, __int128 val) {
  return {v.x - val, v.y - val, v.z - val};
}

P3 operator+(const P3 &v, __int128 val) {
  return {v.x + val, v.y + val, v.z + val};
}

struct V3 {
  P3 start, end;
};

struct Hailstone {
  P3 start, velocity;

  P3 xy_line_eq() const {

    __int128 a = velocity.y;
    __int128 b = -velocity.x;
    __int128 c = velocity.x * start.y - velocity.y * start.x;

    return P3(a, b, c);
  }
};

vector<Hailstone> read_hailstones() {
  vector<Hailstone> hs;
  string line;
  while (getline(cin, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string buf;
    Hailstone h;

    ss >> buf;
    h.start.x = stoll(buf.substr(0, buf.size() - 1));
    ss >> buf;
    h.start.y = stoll(buf.substr(0, buf.size() - 1));
    ss >> buf;
    h.start.z = stoll(buf);

    ss >> buf;

    ss >> buf;
    h.velocity.x = stoll(buf.substr(0, buf.size() - 1));
    ss >> buf;
    h.velocity.y = stoll(buf.substr(0, buf.size() - 1));
    ss >> buf;
    h.velocity.z = stoll(buf);

    hs.push_back(h);
  }
  return hs;
}

__int128 det(__int128 x1, __int128 y1, __int128 x2, __int128 y2) {
  return (x1 * y2) - (y1 * x2);
}

int sign(__int128 x) {
  if (x > 0)
    return 1;
  else if (x < 0)
    return -1;
  else
    return 0;
}

bool check_xy_intersection(const Hailstone &h1, const Hailstone &h2,
                           __int128 min_coord, __int128 max_coord) {

  P3 line_1 = h1.xy_line_eq();
  P3 line_2 = h2.xy_line_eq();

  __int128 zn = det(line_1.x, line_1.y, line_2.x, line_2.y);
  if (zn == 0)
    return false;

  __int128 x = -det(line_1.z, line_1.y, line_2.z, line_2.y) / zn;
  __int128 y = -det(line_1.x, line_1.z, line_2.x, line_2.z) / zn;

  if (
      // check if cross is in the past of h1
      sign(x - h1.start.x) != sign(h1.velocity.x) ||
      sign(y - h1.start.y) != sign(h1.velocity.y)
      // check if cross is in the past of h2
      || sign(x - h2.start.x) != sign(h2.velocity.x) ||
      sign(y - h2.start.y) != sign(h2.velocity.y))
    return false;

  return x >= min_coord && x <= max_coord && y >= min_coord && y <= max_coord;
}

int part_1(const vector<Hailstone> &hs, int64_t min_coord, int64_t max_coord) {

  int num_intersections = 0;

  for (size_t i = 0; i < hs.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (check_xy_intersection(hs[i], hs[j], min_coord, max_coord)) {
        num_intersections++;
      }
    }
  }

  return num_intersections;
}

int64_t part_2(const vector<Hailstone> &hs) {
  z3::context ctx;
  auto start_x = ctx.int_const("start_x");
  auto start_y = ctx.int_const("start_y");
  auto start_z = ctx.int_const("start_z");
  auto vel_x = ctx.int_const("vel_x");
  auto vel_y = ctx.int_const("vel_y");
  auto vel_z = ctx.int_const("vel_z");

  z3::solver solver(ctx);

  vector<string> names(hs.size());

  for (size_t i = 0; i < hs.size(); ++i) {
    names[i] = "t_" + std::to_string(i);
    auto t = ctx.int_const(names[i].c_str());
    auto hs_x = ctx.int_val((int64_t)hs[i].start.x);
    auto hs_y = ctx.int_val((int64_t)hs[i].start.y);
    auto hs_z = ctx.int_val((int64_t)hs[i].start.z);
    auto hv_x = ctx.int_val((int64_t)hs[i].velocity.x);
    auto hv_y = ctx.int_val((int64_t)hs[i].velocity.y);
    auto hv_z = ctx.int_val((int64_t)hs[i].velocity.z);

    solver.add(start_x + vel_x * t == hs_x + hv_x * t);
    solver.add(start_y + vel_y * t == hs_y + hv_y * t);
    solver.add(start_z + vel_z * t == hs_z + hv_z * t);
    solver.add(t > 0);
  }
  auto m = solver.get_model();
  return m.eval(start_x + start_y + start_z, true).as_int64();
}

int main() {
  auto hailstones = read_hailstones();

  // cout << part_1(hailstones, 7, 27) << endl;
  cout << part_1(hailstones, 200000000000000LL, 400000000000000LL) << endl;
  cout << part_2(hailstones) << endl;

  return 0;
}
