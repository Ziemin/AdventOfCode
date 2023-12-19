#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace std::placeholders;

constexpr int64_t MAX_VAL = 4000;
constexpr int64_t MIN_VAL = 1;

size_t att_to_ind(char att) {
  if (att == 'x')
    return 0;
  else if (att == 'm')
    return 1;
  else if (att == 'a')
    return 2;
  else if (att == 's')
    return 3;
  else
    throw std::runtime_error("bad att");
}

struct Range {
  int64_t start, end;

  bool empty() const { return end < start; }
  int64_t size() const { return empty() ? 0 : end - start + 1; }
};

Range rng_intersect(const Range &r1, const Range &r2) {
  return Range{max(r1.start, r2.start), min(r1.end, r2.end)};
}

struct Part {
  array<int64_t, 4> vals;

  int64_t get(char att) const { return vals[att_to_ind(att)]; }
};

ostream &operator<<(ostream &os, const Part &p) {
  os << "x=" << p.vals[0] << ", m=" << p.vals[1] << ", a=" << p.vals[2]
     << ", s=" << p.vals[3];
  return os;
}

struct Check {
  char att;
  char cmp;
  int64_t num;
  string next;

  bool operator()(const Part &part) const {
    int64_t val = part.get(att);
    return (cmp == '<' && val < num) || (cmp == '>' && val > num);
  }

  Range get_range() const {
    if (cmp == '>')
      return {num + 1, MAX_VAL};
    else
      return {1, num - 1};
  }

  Range get_neg_range() const {
    if (cmp == '>')
      return {1, num};
    else
      return {num, MAX_VAL};
  }
};

ostream &operator<<(ostream &os, const Check &ch) {
  os << ch.att << ch.cmp << ch.num << ":" << ch.next;
  return os;
}

enum class NextStep { ACCEPT, REJECT, MOVE };

struct WorkflowResult {
  NextStep step;
  string next;
};

struct Workflow {
  string name;
  vector<Check> checks;
  string final;

  WorkflowResult verify_part(const Part &part) const {
    const string *next = nullptr;
    for (const auto &ch : checks) {
      if (ch(part)) {
        next = &ch.next;
        break;
      }
    }
    if (next == nullptr)
      next = &final;

    if (*next == "A")
      return {NextStep::ACCEPT, ""};
    else if (*next == "R")
      return {NextStep::REJECT, ""};
    else
      return {NextStep::MOVE, *next};
  }
};

ostream &operator<<(ostream &os, const Workflow &wf) {
  os << wf.name << "{";
  for (const Check &ch : wf.checks) {
    os << ch << ",";
  }
  os << wf.final;
  os << "}";
  return os;
}

struct Pipeline {

  Pipeline(vector<Workflow> workflows) {

    for (auto &wf : workflows) {
      auto it = this->workflows.insert({wf.name, std::move(wf)});
      if (it.first->second.name == "in") {
        in_it = it.first;
      }
    }
  }

  unordered_map<string, Workflow> workflows;
  unordered_map<string, Workflow>::const_iterator in_it;

  int64_t get_rating(const Part &p) const {
    auto it = in_it;
    while (true) {
      auto check_res = it->second.verify_part(p);
      switch (check_res.step) {
      case NextStep::ACCEPT:
        return accumulate(begin(p.vals), end(p.vals), 0);
      case NextStep::REJECT:
        return 0;
      case NextStep::MOVE:
        it = workflows.find(check_res.next);
      }
      if (it == workflows.end())
        throw std::runtime_error("non existent workflow: " + check_res.next);
    }
  }
};

ostream &operator<<(ostream &os, const Pipeline &pip) {
  for (const auto &wf : pip.workflows)
    os << wf.second << endl;
  return os;
}

ostream &operator<<(ostream &os, const vector<Part> &parts) {
  for (const auto &p : parts)
    os << p << endl;
  return os;
}

vector<string> split(const string &s, char c) {
  vector<string> results;

  size_t start_ind = 0;
  while (start_ind < s.size()) {
    size_t end_ind = s.find(c, start_ind);
    if (end_ind == string::npos) {
      end_ind = s.size();
    }
    results.push_back(s.substr(start_ind, end_ind - start_ind));
    start_ind = end_ind + 1;
  }

  return results;
}

Workflow parse_workflow(const string &line) {
  Workflow wf;

  size_t left_pos = line.find("{");
  wf.name = line.substr(0, left_pos);
  auto check_strings =
      split(line.substr(left_pos + 1, line.size() - 2 - left_pos), ',');

  wf.final = *check_strings.rbegin();

  for (size_t i = 0; i < check_strings.size() - 1; ++i) {
    const string &cs = check_strings[i];
    char att = cs[0];
    char cmp = cs[1];
    size_t sep_pos = cs.find(':');
    int64_t num = stoll(cs.substr(2, sep_pos - 2));
    string next = cs.substr(sep_pos + 1);

    wf.checks.emplace_back(att, cmp, num, std::move(next));
  }

  return wf;
}

Part parse_part(const string &line) {
  Part part;

  string inner = line.substr(1, line.size() - 2);
  for (size_t i = 0, start_pos = 0; i < 4; i++) {
    size_t end_pos = inner.find(',', start_pos);
    if (end_pos == string::npos)
      end_pos = inner.size();
    part.vals[i] = stoll(inner.substr(start_pos + 2, end_pos - start_pos - 2));
    start_pos = end_pos + 1;
  }

  return part;
}

tuple<Pipeline, vector<Part>> read_input() {
  vector<Workflow> workflows;
  vector<Part> parts;

  string line;
  for (getline(cin, line); !line.empty(); getline(cin, line)) {
    workflows.push_back(parse_workflow(line));
  }
  while (getline(cin, line)) {
    parts.push_back(parse_part(line));
  }

  return {workflows, parts};
}

int64_t part_1(const Pipeline &pipeline, const vector<Part> &parts) {
  int64_t res = 0;
  for (const auto &p : parts)
    res += pipeline.get_rating(p);
  return res;
}

// ===== PART 2 ===========================
struct PartRange {
  array<Range, 4> ranges;

  static PartRange init() {
    return {array<Range, 4>{Range{MIN_VAL, MAX_VAL},
                            {MIN_VAL, MAX_VAL},
                            {MIN_VAL, MAX_VAL},
                            {MIN_VAL, MAX_VAL}}};
  }

  int64_t num_combinations() const {
    int64_t res = 1;
    for (const auto &r : ranges)
      res *= r.size();
    return res;
  }

  PartRange add_range(size_t rng_ind, Range rng) const {
    PartRange new_part_rng = *this;
    new_part_rng.ranges[rng_ind] = rng_intersect(ranges[rng_ind], rng);
    return new_part_rng;
  }

  PartRange add_check(const Check &check) const {
    return add_range(att_to_ind(check.att), check.get_range());
  }

  PartRange add_neg_check(const Check &check) const {
    return add_range(att_to_ind(check.att), check.get_neg_range());
  }
};

vector<PartRange> get_accepted(const Pipeline &pipeline, string wf_name,
                               const PartRange &part_rng) {

  if (wf_name == "A")
    return {part_rng};
  else if (wf_name == "R")
    return {};

  vector<PartRange> accepted;
  PartRange left_rng = part_rng;

  const Workflow &wf = pipeline.workflows.at(wf_name);

  for (const Check &check : wf.checks) {
    PartRange sub_range = left_rng.add_check(check);
    auto sub_acc = get_accepted(pipeline, check.next, sub_range);
    for (auto &acc_pr : sub_acc) {
      accepted.push_back(std::move(acc_pr));
    }

    left_rng = left_rng.add_neg_check(check);
  }

  auto sub_acc = get_accepted(pipeline, wf.final, left_rng);
  for (auto &acc_pr : sub_acc) {
    accepted.push_back(std::move(acc_pr));
  }

  return accepted;
}

int64_t part_2(const Pipeline &pipeline) {
  auto acc_ranges = get_accepted(pipeline, "in", PartRange::init());
  int64_t res = 0;
  for (const auto &rng : acc_ranges)
    res += rng.num_combinations();
  return res;
}

int main() {
  auto [pipeline, parts] = read_input();
  // cout << pipeline << endl;
  // cout << parts << endl;

  cout << part_1(pipeline, parts) << endl;
  cout << part_2(pipeline) << endl;

  return 0;
}
