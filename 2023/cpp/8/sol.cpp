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

uint64_t part_1(const string &cycle, const unordered_map<string, edge> &graph) {
  uint64_t steps = 0;

  auto it = graph.find("AAA");

  while (it->first != "ZZZ") {

    char dir = cycle[steps % cycle.size()];
    if (dir == 'L')
      it = graph.find(it->second.left);
    else
      it = graph.find(it->second.right);

    steps += 1;
  }

  return steps;
}

int main() {

  iostream::sync_with_stdio(true);
  auto [cycle, graph] = read_input();
  cout << "part_1 " << part_1(cycle, graph) << endl;

  return 0;
}
