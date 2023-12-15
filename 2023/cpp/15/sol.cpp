#include <cctype>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <sstream>
#include <vector>

using namespace std;

vector<string> read_input() {
  vector<string> instructions;

  string line;
  stringstream ss;
  while (getline(cin, line)) {
    if (!line.empty())
      ss << line;
  }
  for (string s; getline(ss, s, ',');)
    instructions.push_back(std::move(s));

  return instructions;
}

uint64_t aoc_hash(const string &s) {
  uint64_t current = 0;
  for (char c : s) {
    current += (uint64_t)c;
    current *= 17;
    current %= 256;
  }
  return current;
}

uint64_t part_1(const vector<string> &instructions) {

  uint64_t result = 0;
  for (const auto &s : instructions) {
    result += aoc_hash(s);
  }

  return result;
}

tuple<string, char, uint> parse_instruction(const string &instr) {
  string label;
  char op;
  uint lens = 0;
  for (char c : instr) {
    if (isalpha(static_cast<unsigned char>(c)))
      label.push_back(c);
    else if (isdigit(static_cast<unsigned char>(c)))
      lens = c - '0';
    else
      op = c;
  }
  return {label, op, lens};
}

uint64_t part_2(const vector<string> &instructions) {

  vector<tuple<string, uint64_t>> boxes[256];
  for (const auto &s : instructions) {
    auto [label, op, lens] = parse_instruction(s);
    auto &box = boxes[aoc_hash(label)];
    auto it = ranges::find_if(
        box, [&label](const auto &t) { return get<0>(t) == label; });

    if (op == '=') {
      if (it != box.end()) {
        get<1>(*it) = lens;
      } else {
        box.push_back({label, lens});
      }
    } else {
      if (it != box.end())
        box.erase(it);
    }
  }

  uint64_t result = 0;
  for (uint64_t b = 0; b < 256; ++b) {
    for (uint64_t s = 0; s < boxes[b].size(); ++s) {
      result += (b + 1) * (s + 1) * get<1>(boxes[b][s]);
    }
  }

  return result;
}

int main() {
  auto instructions = read_input();
  cout << part_1(instructions) << endl;
  cout << part_2(instructions) << endl;

  return 0;
}
