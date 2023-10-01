//#define CATCH_CONFIG_MAIN

#include "bitfield.hpp"
#include "catch2/catch_test_macros.hpp"

#include <sstream>

TEST_CASE("Initial", "[BitMap]") {
  BitMap bitmap(10, false);

  bitmap.SetBitValue(8, true);
  REQUIRE(bitmap.GetBitValue(8) == true);
}

TEST_CASE("Initial With String", "[BitMap]") {
  std::string bits{"1 0 1 0 1 0 1 0 1"};
  BitMap bitmap(std::move(ConvertBitsStringToBitMap(bits)));
  REQUIRE(bitmap.Size() == 9);
  REQUIRE(bitmap.GetBitValue(0) == true);
  REQUIRE(bitmap.GetBitValue(1) == false);
}

TEST_CASE("Initial With String with 0001 000", "[BitMap]") {
  std::string bits{"001 00 1 0000 1 0 1 0 1"};
  BitMap bitmap(std::move(ConvertBitsStringToBitMap(bits)));
  REQUIRE(bitmap.Size() != 9);
  REQUIRE(bitmap.GetBitValue(0) == false);
  REQUIRE(bitmap.GetBitValue(1) == false);
  REQUIRE(bitmap.GetBitValue(2) == true);
}

TEST_CASE("Out put", "[BitMap]") {
  std::string bits{"1 0 1 0 1 0 1 0 1"};
  BitMap bitmap(std::move(ConvertBitsStringToBitMap(bits)));
  REQUIRE(bitmap.Size() == 9);
  REQUIRE(bitmap.GetBitValue(0) == true);
  REQUIRE(bitmap.GetBitValue(1) == false);
}
