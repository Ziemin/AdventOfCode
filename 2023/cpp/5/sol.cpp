#include <algorithm>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <ranges>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

tuple<vector<uint64_t>, vector<vector<tuple<uint64_t, uint64_t, uint64_t>>>>
read_input() {

  vector<uint64_t> seeds;
  vector<vector<tuple<uint64_t, uint64_t, uint64_t>>> maps;
  string line, buffer;
  uint64_t num;

  // read seeds
  getline(cin, line);
  istringstream ss(line);
  ss >> buffer;
  while (!ss.eof()) {
    ss >> num;
    seeds.push_back(num);
  }

  vector<tuple<uint64_t, uint64_t, uint64_t>> current_map;
  // read maps
  while (getline(cin, line)) {
    if (line.empty()) {
      ranges::sort(current_map);
      maps.push_back(std::move(current_map));
    } else if (line.find("-to-") != string::npos) {
      continue;
    } else {
      uint64_t dest, source, len;
      istringstream ss(line);
      ss >> dest >> source >> len;
      current_map.emplace_back(source, dest, len);
    }
  }

  if (!current_map.empty()) {
    ranges::sort(current_map);
    maps.push_back(std::move(current_map));
  }

  return {seeds, maps};
}

uint64_t
part_1(const vector<uint64_t> &seeds,
       const vector<vector<tuple<uint64_t, uint64_t, uint64_t>>> &maps) {

  auto min_loc = numeric_limits<uint64_t>::max();

  for (auto seed : seeds) {
    uint64_t ind = seed;

    for (const auto &rng_map : maps) {
      // the arrays are so small binsearch is irrelevant
      for (const auto &[source, dest, len] : rng_map) {
        if (source <= ind && ind < source + len) {
          ind = dest + ind - source;
          break;
        }
      }
    }

    min_loc = min(ind, min_loc);
  }

  return min_loc;
}

uint64_t
part_2(const vector<uint64_t> &seeds,
       const vector<vector<tuple<uint64_t, uint64_t, uint64_t>>> &maps) {

  function<uint64_t(uint64_t, uint64_t, int)> rng_search = [&](uint64_t start,
                                                               uint64_t end,
                                                               int map_ind) {
    // cout << "start " << start << " end " << end << endl;
    if (map_ind == maps.size())
      return start;

    auto min_ind = numeric_limits<uint64_t>::max();

    for (const auto &[source, dest, len] : maps[map_ind]) {

      uint64_t int_start = max(start, source), int_end = min(end, source + len);
      bool has_intersection = int_start < int_end;

      if (has_intersection) {
        if (start < source)
          min_ind = min(min_ind, rng_search(start, int_start, map_ind + 1));

        uint64_t diff = int_start - source;
        min_ind = min(min_ind, rng_search(dest + diff,
                                          dest + diff + (int_end - int_start),
                                          map_ind + 1));

        start = int_end;
        end = max(end, int_end);
        if (start == end)
          break;
      } else if (start < source) {
        break;
      }
    }

    if (start < end) {
      min_ind = min(min_ind, rng_search(start, end, map_ind + 1));
    }

    // cout << "min_ind " << min_ind << endl;
    return min_ind;
  };

  auto min_loc = numeric_limits<uint64_t>::max();
  for (int i = 0; i < seeds.size(); i += 2) {
    min_loc = min(rng_search(seeds[i], seeds[i] + seeds[i + 1], 0), min_loc);
  }

  return min_loc;
}

int main() {
  auto [seeds, maps] = read_input();
  cout << part_1(seeds, maps) << endl;
  cout << part_2(seeds, maps) << endl;
  return 0;
}
