#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

tuple<vector<int64_t>, vector<int64_t>> read_inputs() {
  vector<int64_t> times, records;

  string times_line, records_line, buf;
  getline(cin, times_line);
  getline(cin, records_line);

  istringstream ts(times_line);
  ts >> buf;
  while (!ts.eof()) {
    int64_t t;
    ts >> t;
    times.push_back(t);
  }

  istringstream rs(records_line);
  rs >> buf;
  while (!rs.eof()) {
    int64_t r;
    rs >> r;
    records.push_back(r);
  }

  return {times, records};
}

int64_t part_1(const vector<int64_t> &times, const vector<int64_t> &records) {

  int64_t res = 1;

  for (int i = 0; i < times.size(); i++) {
    auto tm = times[i], rec = records[i];

    double dlt = tm * tm - 4 * rec;
    double x1 = (tm - sqrt(dlt)) / 2.0;
    double x2 = (tm + sqrt(dlt)) / 2.0;

    int64_t l = ceil(x1);
    int64_t r = int64_t(x2);

    while ((tm - l) * l <= rec)
      l++;
    while ((tm - r) * r <= rec)
      r--;

    res *= (r - l + 1);
  }
  return res;
}

int64_t part_2(const vector<int64_t> &times, const vector<int64_t> &records) {

  string tm_str = "", rec_str = "";
  for (int i = 0; i < times.size(); i++) {
    tm_str += to_string(times[i]);
    rec_str += to_string(records[i]);
  }

  int64_t tm = stoll(tm_str), rec = stoll(rec_str);

  double dlt = tm * tm - 4 * rec;
  double x1 = (tm - sqrt(dlt)) / 2.0;
  double x2 = (tm + sqrt(dlt)) / 2.0;

  int64_t l = ceil(x1);
  int64_t r = int64_t(x2);

  while ((tm - l) * l <= rec)
    l++;
  while ((tm - r) * r <= rec)
    r--;

  return r - l + 1;
}

int main() {
  auto [times, records] = read_inputs();
  cout << part_1(times, records) << endl;
  cout << part_2(times, records) << endl;
  return 0;
}
