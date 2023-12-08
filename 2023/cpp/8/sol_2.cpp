#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct edge {
  string left, right;
};

tuple<string, unordered_map<string, edge>> read_input() {

  string cycle;
  getline(cin, cycle);

  unordered_map<string, edge> graph;
  string line;
  while (getline(cin, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string source;
    char buf[10];
    ss >> source;

    ss.ignore(4);
    ss.read(buf, 3);
    string left(buf, buf + 3);

    ss.ignore(2);
    ss.read(buf, 3);
    string right(buf, buf + 3);

    graph[source] = {left, right};
  }

  return {cycle, graph};
}

uint64_t gcd(uint64_t a, uint64_t b) {

  if (a < b)
    swap(a, b);

  while (b != 0) {
    uint64_t c = b;
    b = a % b;
    a = c;
  }

  return a;
}

uint64_t lcm(uint64_t a, uint64_t b) { return (a * b) / gcd(a, b); }

uint64_t part_2(const string &cycle, const unordered_map<string, edge> &graph) {

  uint64_t res = 1;

  for (auto start_it = graph.cbegin(); start_it != graph.cend(); ++start_it) {
    if (start_it->first[2] != 'A')
      continue;

    uint64_t st = 0;
    auto it = start_it;

    while (it->first[2] != 'Z') {

      char dir = cycle[st % cycle.size()];
      if (dir == 'L')
        it = graph.find(it->second.left);
      else
        it = graph.find(it->second.right);

      st += 1;
    }

    res = lcm(res, st);
  }
  return res;
}

int main() {

  iostream::sync_with_stdio(true);
  auto [cycle, graph] = read_input();
  cout << "part_2 " << part_2(cycle, graph) << endl;

  return 0;
}
