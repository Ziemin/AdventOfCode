#include <cassert>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

enum class ModuleType { BROADCASTER, FLIP_FLOP, CONJUNCTION };
enum class Pulse { NO, LOW, HIGH };

struct Signal {
  Pulse val;
  string source;
  string target;
};

struct Module {
  ModuleType mt;
  vector<string> outputs;
  unordered_map<string, Pulse> last_inputs;
  bool is_on = false;

  Pulse process_signal(const Signal &sgn) {
    switch (mt) {
    case ModuleType::BROADCASTER:
      return sgn.val;

    case ModuleType::FLIP_FLOP:
      if (sgn.val == Pulse::LOW) {
        is_on = is_on ? false : true;
        return is_on ? Pulse::HIGH : Pulse::LOW;
      } else {
        return Pulse::NO;
      }
      break;

    case ModuleType::CONJUNCTION: {
      last_inputs[sgn.source] = sgn.val;
      bool all_high = true;
      for (auto &[_, last_pulse] : last_inputs)
        all_high &= last_pulse == Pulse::HIGH;
      is_on = all_high;
      return all_high ? Pulse::LOW : Pulse::HIGH;
    }
    }

    return Pulse::NO;
  }
};

ostream &operator<<(ostream &os, const Module &md) {
  switch (md.mt) {
  case (ModuleType::BROADCASTER):
    os << "broadcaster";
    break;

  case (ModuleType::FLIP_FLOP):
    os << "flip_flop:" << md.is_on;
    break;

  case (ModuleType::CONJUNCTION):
    os << "conjunction:";
    for (const auto &[name, last_val] : md.last_inputs) {
      os << (last_val == Pulse::HIGH ? 1 : 0);
    }
    break;
  }
  return os;
};

struct Network {
  unordered_map<string, Module> modules;

  tuple<int64_t, int64_t, bool>
  push_button(const string &check_module_is_on = "") {
    int64_t num_high = 0, num_low = 0;

    queue<Signal> que;
    que.push(Signal(Pulse::LOW, "", "broadcaster"));

    while (!que.empty()) {
      Signal sig = que.front();
      que.pop();

      if (sig.val == Pulse::HIGH)
        num_high += 1;
      else
        num_low += 1;

      Module &md = modules[sig.target];

      Pulse out_pulse = md.process_signal(sig);

      if (sig.target == check_module_is_on && !md.is_on) {
        return {num_high, num_low, false};
      }

      if (out_pulse != Pulse::NO) {
        for (auto &target : md.outputs) {
          que.push(Signal(out_pulse, sig.target, target));
        }
      }
    }
    return {num_high, num_low, true};
  }

  string get_state() const {
    stringstream ss;
    for (const auto &[name, md] : modules)
      ss << name << "=" << md << ", ";
    return ss.str();
  }
};

Network read_input() {
  string line;
  Network net;

  while (getline(cin, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);

    string src;
    string skip;
    string trgt;
    string module_name;
    Module md;

    ss >> src;
    if (src == "broadcaster") {
      md.mt = ModuleType::BROADCASTER;
      module_name = src;
    } else if (src[0] == '%') {
      md.mt = ModuleType::FLIP_FLOP;
      module_name = src.substr(1);
    } else if (src[0] == '&') {
      md.mt = ModuleType::CONJUNCTION;
      module_name = src.substr(1);
    } else {
      throw std::runtime_error("bad module type");
    }

    ss >> skip;
    while (ss) {
      ss >> trgt;
      if (trgt.empty())
        continue;
      if (*trgt.rbegin() == ',')
        trgt = trgt.substr(0, trgt.size() - 1);
      assert(trgt != "");
      md.outputs.push_back(std::move(trgt));
    }
    assert(module_name != "");
    net.modules[module_name] = md;
  }

  for (auto &[name, md] : net.modules) {
    for (auto &trgt : md.outputs) {
      net.modules[trgt].last_inputs[name] = Pulse::LOW;
    }
  }

  return net;
}

tuple<int64_t, int64_t> solve(const Network &net, int64_t steps) {
  Network proc_net = net;
  vector<int64_t> sum_lows, sum_highs;
  unordered_map<string, size_t> state_to_ind;

  int64_t cycle_start_ind = -1;

  for (int64_t s = 0; s < steps; ++s) {
    auto [high, low, _] = proc_net.push_button();
    string net_state = proc_net.get_state();
    auto it = state_to_ind.find(net_state);
    if (it == state_to_ind.end()) {
      sum_highs.push_back(high + (sum_highs.empty() ? 0 : *sum_highs.rbegin()));
      sum_lows.push_back(low + (sum_lows.empty() ? 0 : *sum_lows.rbegin()));
      state_to_ind[net_state] = sum_highs.size() - 1;
    } else {
      cycle_start_ind = it->second;
      break;
    }
  }

  if (cycle_start_ind == -1) {
    return {*sum_lows.rbegin(), *sum_highs.rbegin()};
  } else {
    int64_t sum_pref_high =
        cycle_start_ind > 0 ? sum_highs[cycle_start_ind - 1] : 0;
    int64_t sum_pref_low =
        cycle_start_ind > 0 ? sum_lows[cycle_start_ind - 1] : 0;
    int64_t cycle_length = sum_highs.size() - cycle_start_ind;

    int64_t cycle_sum_high = *sum_highs.rbegin() - sum_pref_high;
    int64_t cycle_sum_low = *sum_lows.rbegin() - sum_pref_low;

    int64_t left_steps = steps - cycle_start_ind;

    int64_t num_full_cycles = left_steps / cycle_length;
    int64_t remainder = left_steps % cycle_length;

    int64_t remainder_sum_high =
        sum_highs[cycle_start_ind + remainder] - sum_highs[cycle_start_ind];
    int64_t remainder_sum_low =
        sum_lows[cycle_start_ind + remainder] - sum_lows[cycle_start_ind];

    int64_t final_sum_high =
        sum_pref_high + num_full_cycles * cycle_sum_high + remainder_sum_high;
    int64_t final_sum_low =
        sum_pref_low + num_full_cycles * cycle_sum_low + remainder_sum_low;

    return {final_sum_high, final_sum_low};
  }
}

int64_t gcd(int64_t a, int64_t b) {

  if (a < b)
    swap(a, b);

  while (b != 0) {
    int64_t c = b;
    b = a % b;
    a = c;
  }

  return a;
}

int64_t lcm(int64_t a, int64_t b) { return (a * b) / gcd(a, b); }

int64_t solve_2(const Network &net) {

  int64_t num_required = 1;

  vector<string> end_module_names;
  // I gave up on general case solution. Here is the one which realizes
  // that the input graph is very special and requires 4 nodes to be off at some
  // point.
  for (const auto &[name, md] :
       net.modules.at(net.modules.at("rx").last_inputs.begin()->first)
           .last_inputs) {

    Network state = net;
    int64_t num_pushes = 0;
    while (true) {
      auto [l, h, is_on] = state.push_button(name);
      num_pushes++;
      if (!is_on)
        break;
    }
    num_required = lcm(num_required, num_pushes);
  }

  return num_required;
}

int main() {
  Network net = read_input();

  for (int64_t steps : vector<int64_t>({1, 1000})) {
    auto [high, low] = solve(net, steps);
    cout << steps << ": " << high << " * " << low << " = " << high * low
         << endl;
  }
  if (net.modules.contains("rx")) {
    cout << solve_2(net) << endl;
  }

  return 0;
}
