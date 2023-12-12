#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

vector<tuple<string, vector<int>>> read_input() {
  string line;
  vector<tuple<string, vector<int>>> input;

  while (getline(cin, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string arr;
    ss >> arr;

    vector<int> seqs;

    string nums;
    ss >> nums;
    int beg = 0;
    int end = 0;
    while (beg < nums.size()) {
      if (end >= nums.size() || nums[end] == ',') {
        seqs.push_back(stoi(nums.substr(beg, end - beg)));
        beg = end + 1;
      }
      ++end;
    }

    input.emplace_back(std::move(arr), std::move(seqs));
  }

  return input;
}

int64_t num_ways(const string &arr, const vector<int> &seq) {

  int A = arr.size(), S = seq.size();

  vector<vector<int64_t>> nm(A + 1, vector<int64_t>(S + 1, 0));

  nm[0][0] = 1;

  for (int j = 1; j <= A; j++) {
    if (arr[j - 1] == '#')
      break;
    nm[j][0] = 1;
  }

  for (int s = 1; s <= S; ++s) {
    for (int a = 1; a <= A; ++a) {

      if (arr[a - 1] == '.') {
        nm[a][s] = nm[a - 1][s];

      } else {
        // CASE 1: if we have # or change ? into #

        int k = seq[s - 1];

        bool has_consecutive = true;
        // a is too short to squeeze in k #s
        if (a < k)
          has_consecutive = false;
        else {
          // check if the last k characters are not .
          for (int j = a - k + 1; j < a; ++j) {
            if (arr[j - 1] == '.')
              has_consecutive = false;
          }
        }

        if (has_consecutive) {
          if (a > k) {
            // we fill the first a - k- 1
            if (arr[a - k - 1] != '#') {
              nm[a][s] = nm[a - k - 1][s - 1];
            }
          } else {
            // a == k, we'll get  x only if s == 1
            nm[a][s] = nm[a - k][s - 1];
          }
        }

        // CASE 2: we change ? into .
        if (arr[a - 1] == '?') {
          nm[a][s] += nm[a - 1][s];
        }
      }
    }
  }

  return nm[A][S];
}

int64_t part_1(const vector<tuple<string, vector<int>>> &cases) {

  int64_t res = 0;

  for (const auto &[arr, seq] : cases) {
    res += num_ways(arr, seq);
  }

  return res;
}

int64_t part_2(const vector<tuple<string, vector<int>>> &cases) {

  int64_t res = 0;

  for (const auto &[arr, seq] : cases) {
    string new_arr = arr;
    vector<int> new_seq = seq;
    for (int i = 0; i < 4; i++) {
      new_arr += "?" + arr;
      copy(begin(seq), end(seq), back_inserter(new_seq));
    }

    res += num_ways(new_arr, new_seq);
  }

  return res;
}

int main() {

  auto cases = read_input();

  cout << part_1(cases) << endl;
  cout << part_2(cases) << endl;
  return 0;
}
