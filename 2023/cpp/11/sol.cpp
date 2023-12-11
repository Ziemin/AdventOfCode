#include <cctype>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

vector<string> read_input() {
  string line;
  vector<string> cosmos;

  while (getline(cin, line)) {
    if (!line.empty())
      cosmos.push_back(line);
  }

  return cosmos;
}

struct PrefSums {
  vector<int64_t> col_gal;
  vector<int64_t> row_gal;

  int64_t dist(tuple<int, int> first, tuple<int, int> second) const {
    auto [f_r, f_c] = first;
    auto [s_r, s_c] = second;

    if (f_r > s_r)
      swap(f_r, s_r);
    if (f_c > s_c)
      swap(f_c, s_c);

    int64_t num_g_r = row_gal[s_r] - row_gal[f_r];
    int64_t num_g_c = col_gal[s_c] - col_gal[f_c];

    return (s_r - f_r + num_g_r) + (s_c - f_c + num_g_c);
  }
};

PrefSums get_pref_sums(const vector<string> &cosmos, int64_t fill) {
  int N = cosmos.size(), M = cosmos[0].size();

  vector<int64_t> col_gal(M, 0);
  vector<int64_t> row_gal(N, 0);

  for (int r = 0; r < N; ++r) {
    for (int c = 0; c < M; ++c) {
      row_gal[r] = max(row_gal[r], (int64_t)(cosmos[r][c] == '#' ? 1 : 0));
    }
  }
  for (int r = 0; r < N; ++r)
    row_gal[r] = (fill - 1) * ((row_gal[r] + 1) % 2);
  for (int r = 1; r < N; ++r)
    row_gal[r] = row_gal[r - 1] + row_gal[r];

  for (int c = 0; c < M; ++c) {
    for (int r = 0; r < N; ++r) {
      col_gal[c] = max(col_gal[c], (int64_t)(cosmos[r][c] == '#' ? 1 : 0));
    }
  }
  for (int c = 0; c < M; ++c)
    col_gal[c] = (fill - 1) * ((col_gal[c] + 1) % 2);
  for (int c = 1; c < M; ++c)
    col_gal[c] = col_gal[c - 1] + col_gal[c];

  return {col_gal, row_gal};
}

uint64_t solution(const vector<string> &cosmos, int64_t fill) {
  int N = cosmos.size(), M = cosmos[0].size();

  vector<tuple<int, int>> galaxies;
  for (int r = 0; r < N; ++r) {
    for (int c = 0; c < M; ++c) {
      if (cosmos[r][c] == '#')
        galaxies.push_back({r, c});
    }
  }

  auto pref_sums = get_pref_sums(cosmos, fill);

  int64_t sum_distances = 0;

  for (int i = 0; i < galaxies.size(); ++i)
    for (int j = 0; j < i; ++j) {
      sum_distances += pref_sums.dist(galaxies[i], galaxies[j]);
    }

  return sum_distances;
}

int main() {
  auto cosmos = read_input();

  cout << solution(cosmos, 2) << endl;
  cout << solution(cosmos, 10) << endl;
  cout << solution(cosmos, 100) << endl;
  cout << solution(cosmos, 1000000) << endl;

  return 0;
}
