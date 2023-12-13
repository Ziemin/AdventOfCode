#include <iostream>
#include <vector>

using namespace std;

vector<vector<string>> read_input() {
  vector<vector<string>> patterns;

  string line;
  vector<string> ptrn;
  while (getline(cin, line)) {
    if (line.empty()) {
      if (!ptrn.empty()) {
        patterns.push_back(std::move(ptrn));
      }
    } else {
      ptrn.push_back(std::move(line));
    }
  }
  if (!ptrn.empty())
    patterns.push_back(std::move(ptrn));

  return patterns;
}

int64_t get_reflection(const vector<string> &ptrn, int diff) {
  const int C = ptrn[0].size();

  for (int r = 0; r < ptrn.size() - 1; ++r) {
    int sum_diffs = 0;

    int len = min(r + 1, (int)ptrn.size() - r - 1);
    for (int i = 0; i < len; ++i) {
      for (int j = 0; j < C; ++j) {
        sum_diffs += (int)(ptrn[r - i][j] != ptrn[r + i + 1][j]);
      }
    }

    if (sum_diffs == diff)
      return r + 1;
  }
  return 0;
}

vector<string> transpose(const vector<string> ptrn) {

  vector<string> transposed;
  for (int c = 0; c < ptrn[0].size(); ++c) {
    string col;

    for (int r = 0; r < ptrn.size(); ++r)
      col.push_back(ptrn[r][c]);

    transposed.push_back(col);
  }

  return transposed;
}

int64_t solve(const vector<vector<string>> &patterns, int diff) {

  int64_t res = 0;
  for (const auto &ptrn : patterns) {
    res += 100 * get_reflection(ptrn, diff) +
           get_reflection(transpose(ptrn), diff);
  }
  return res;
}

int main() {
  auto patterns = read_input();
  cout << solve(patterns, 0) << endl;
  cout << solve(patterns, 1) << endl;

  return 0;
}
