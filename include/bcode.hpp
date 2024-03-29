// Copyright 2021 Uttep.
// License(BSD)
// Author: Uttep
// Bittorrent Protocol Code Parse
#ifndef BITUSK_SRC_BCODE_H
#define BITUSK_SRC_BCODE_H
#include <bits/c++config.h>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace bitusk {

using namespace std;

class Bcode {
  typedef string::iterator position;

 public:
  static string extractSegment(position&, const char);
  static bool findPair(position& pos);
  static size_t parseInt(position&);
  // Get String Type bcode. Reading intervel [begin, end), pos will
  // change by this rule.
  // Pass a reference of position and
  // return one string.
  static string parseStr(position& pos);
};
}  // namespace bitusk

#endif  // BITUSK_SRC_BCODE_H
