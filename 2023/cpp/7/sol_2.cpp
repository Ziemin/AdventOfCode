#include <algorithm>
#include <bits/ranges_cmp.h>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <tuple>
#include <vector>

using namespace std;

vector<tuple<string, uint64_t>> read_inputs() {
  vector<tuple<string, uint64_t>> cards;

  while (!cin.eof()) {
    string hand;
    uint64_t bid;
    cin >> hand >> bid;
    if (hand.empty())
      break;
    cards.emplace_back(std::move(hand), bid);
  }

  return cards;
}

constexpr int NUM_CARDS = 13;

constexpr int card_to_strength(char card) {
  switch (card) {
  case 'J':
    return 0;
  case '2':
    return 1;
  case '3':
    return 2;
  case '4':
    return 3;
  case '5':
    return 4;
  case '6':
    return 5;
  case '7':
    return 6;
  case '8':
    return 7;
  case '9':
    return 8;
  case 'T':
    return 9;
  case 'Q':
    return 10;
  case 'K':
    return 11;
  case 'A':
    return 12;
  default:
    exit(1);
  }
}

enum class hand_strength : uint64_t {
  high_card = 1,
  one_pair = 2,
  two_pair = 3,
  three_of_a_kind = 4,
  full_house = 5,
  four_of_a_kind = 6,
  five_of_a_kind = 7
};

struct Hand {

  string cards;
  hand_strength strength;
};

hand_strength get_strength(const string &cards) {
  vector<int> counts(NUM_CARDS, 0);
  for (char c : cards) {
    counts[card_to_strength(c)]++;
  }

  ranges::sort(counts, ranges::greater());

  if (counts[0] == 1)
    return hand_strength::high_card;
  else if (counts[0] == 2) {
    if (counts[1] == 2)
      return hand_strength::two_pair;
    else
      return hand_strength::one_pair;
  } else if (counts[0] == 3) {
    if (counts[1] == 2)
      return hand_strength::full_house;
    else
      return hand_strength::three_of_a_kind;
  } else if (counts[0] == 4)
    return hand_strength::four_of_a_kind;
  else if (counts[0] == 5)
    return hand_strength::five_of_a_kind;
  else {
    cerr << "unexpected count " << counts[0] << endl;
    exit(2);
  }
}

hand_strength get_best_strength(const string &cards) {
  hand_strength best = get_strength(cards);
  if (cards.find_first_of('J') == string::npos)
    return best;

  for (char c : string("23456789TQKA")) {
    string changed_cards = cards;
    replace(changed_cards.begin(), changed_cards.end(), 'J', c);
    best = max(best, get_strength(changed_cards));
  }
  return best;
}

bool compare_hands(const Hand &one, const Hand &two) {
  if (one.strength == two.strength) {
    for (int i = 0; i < one.cards.size(); i++) {
      if (one.cards[i] != two.cards[i]) {
        return card_to_strength(one.cards[i]) < card_to_strength(two.cards[i]);
      }
    }
    return false;
  } else {
    return one.strength < two.strength;
  }
}

uint64_t part_2(const vector<tuple<string, uint64_t>> &cards_and_bids) {
  vector<tuple<Hand, uint64_t>> hands_and_bids;

  for (const auto &[cards, bid] : cards_and_bids) {
    hands_and_bids.emplace_back(Hand{cards, get_best_strength(cards)}, bid);
  }

  ranges::sort(hands_and_bids, [](const auto &one, const auto &two) {
    return compare_hands(get<0>(one), get<0>(two));
  });

  uint64_t result = 0;
  for (uint64_t rank = 1; rank <= hands_and_bids.size(); rank++) {
    result += get<1>(hands_and_bids[rank - 1]) * rank;
  }

  return result;
}

int main() {

  const auto cards = read_inputs();
  cout << part_2(cards) << endl;

  return 0;
}
