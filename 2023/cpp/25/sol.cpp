#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

using RawEdge = tuple<string, string>;

struct Graph {
  unordered_map<size_t, vector<size_t>> edges;
  vector<size_t> cardinalities;

  unordered_map<string, size_t> name_to_ind;

  size_t n() const { return edges.size(); }

  void remove_directed_edge(size_t v1, size_t v2) {
    auto &v1_edges = edges[v1];
    v1_edges.erase(remove(v1_edges.begin(), v1_edges.end(), v2),
                   v1_edges.end());
  }

  void remove_edge(size_t v1, size_t v2) {
    remove_directed_edge(v1, v2);
    remove_directed_edge(v2, v1);
  }

  void add_edge(size_t v1, size_t v2) {
    edges[v1].push_back(v2);
    edges[v2].push_back(v1);
  }

  void contract(size_t v1, size_t v2) {
    // remove v2 from v1's adjacency list
    remove_directed_edge(v1, v2);

    // add v2's neighbors except for v1 to v1
    for (size_t v_n : edges[v2]) {
      if (v_n == v1)
        continue;
      remove_directed_edge(v_n, v2);
      add_edge(v1, v_n);
    }

    // increase v1's cardinality by 1
    cardinalities[v1] += cardinalities[v2];

    // remove v2
    edges.erase(edges.find(v2));
  }

  tuple<size_t, size_t> choose_random_edge(std::mt19937 &gen) const {
    int num_edges = 0;
    for (const auto &[_, neighs] : edges) {
      num_edges += neighs.size();
    }

    std::uniform_int_distribution<> distr(0, num_edges - 1);
    int edge_ind = distr(gen);
    int sum_prev = 0;
    for (const auto &[v1, neighs] : edges) {
      int new_sum = sum_prev + neighs.size();

      if (new_sum > edge_ind) {
        size_t v2 = neighs[edge_ind - sum_prev];
        return {v1, v2};
      }

      sum_prev = new_sum;
    }

    assert(false);
  }

  static Graph from(const vector<RawEdge> &raw_edges) {

    Graph graph;

    for (const auto &[name_1, name_2] : raw_edges) {
      auto v1_it = graph.name_to_ind.find(name_1);
      if (v1_it == graph.name_to_ind.end()) {
        v1_it =
            graph.name_to_ind.insert({name_1, graph.name_to_ind.size()}).first;
        graph.cardinalities.push_back(1);
      }
      size_t v1 = v1_it->second;

      auto v2_it = graph.name_to_ind.find(name_2);
      if (v2_it == graph.name_to_ind.end()) {
        v2_it =
            graph.name_to_ind.insert({name_2, graph.name_to_ind.size()}).first;
        graph.cardinalities.push_back(1);
      }
      size_t v2 = v2_it->second;

      graph.edges[v1].push_back(v2);
      graph.edges[v2].push_back(v1);
    }

    return graph;
  }
};

vector<RawEdge> read_raw_edges() {
  vector<RawEdge> raw_edges;

  string line;
  while (getline(cin, line)) {
    if (line.empty())
      continue;

    stringstream ss(line);
    string source;
    ss >> source;
    source.pop_back();

    string target;
    while (ss) {
      ss >> target;
      if (target.empty())
        continue;
      raw_edges.push_back({source, std::move(target)});
    }
  }

  return raw_edges;
}

size_t part_1(const Graph &graph, const vector<RawEdge> &raw_edges) {

  std::random_device rd;
  std::mt19937 gen(rd());

  while (true) {
    Graph g = graph;
    while (g.n() != 2) {
      auto [v1, v2] = g.choose_random_edge(gen);
      g.contract(v1, v2);
    }

    auto v1_it = g.edges.begin();
    size_t v1 = v1_it->first;
    if (v1_it->second.size() != 3) {
      // cut should be 3 edges
      continue;
    }
    size_t first_component = g.cardinalities[v1];
    size_t second_component = g.cardinalities[v1_it->second[0]];
    assert(first_component + second_component == graph.n());
    return first_component * second_component;
  }

  return -1;
}

int main() {
  auto raw_edges = read_raw_edges();
  auto graph = Graph::from(raw_edges);
  cout << part_1(graph, raw_edges) << endl;
  return 0;
}
