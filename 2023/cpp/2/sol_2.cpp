#include <algorithm>
#include <bits/ranges_algo.h>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

int main() {

  string line;
  uint64_t sum_ids = 0;
  while (getline(cin, line)) {

    istringstream iss(line);
    iss.ignore(5);
    uint64_t game_id;
    iss >> game_id;
    iss.ignore(2);

    uint64_t blue = 0, red = 0, green = 0;
    string color;
    while (!iss.eof()) {
      uint64_t num = 0;
      iss >> num;
      iss >> color;
      if (color.starts_with("green"))
        green = max(green, num);
      else if (color.starts_with("blue"))
        blue = max(blue, num);
      if (color.starts_with("red"))
        red = max(red, num);
    }

    sum_ids += (red * blue * green);
  }

  cout << sum_ids << endl;

  return 0;
}
