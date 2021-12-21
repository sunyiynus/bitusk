//#define CATCH_CONFIG_MAIN

#include "bitfield.hpp"
#include "catch.hpp"


#include <sstream>

TEST_CASE("Initial", "[BitMap]") {
    BitMap bitmap(10);
    REQUIRE( bitmap.Counting(0) == 10);

    SECTION("Test BitMap::BitMap(size_t)") {
        bitmap.SetBitValue(8,true);
        REQUIRE( bitmap.GetBitValue(8) == true);
    }
}


TEST_CASE("Initial With String", "[BitMap]") {
    std::string bits {"1 0 1 0 1 0 1 0 1"};
    BitMap bitmap (bits);
    REQUIRE( bitmap.Size() ==  9);
    REQUIRE( bitmap.GetBitValue(0) ==  true);
    REQUIRE( bitmap.GetBitValue(1) ==  false);
    REQUIRE( bitmap.Counting(true) == 5);
    REQUIRE( bitmap.Counting(false) == 4);
    REQUIRE( (bitmap.Counting(false) + bitmap.Counting(true)) == bitmap.Size());
}


TEST_CASE("Initial With String with 0001 000", "[BitMap]") {
    std::string bits {"001 00 1 0000 1 0 1 0 1"};
    BitMap bitmap (bits);
    REQUIRE( bitmap.Size() ==  9);
    REQUIRE( bitmap.GetBitValue(0) ==  true);
    REQUIRE( bitmap.GetBitValue(1) ==  false);
    REQUIRE( bitmap.Counting(true) == 5);
    REQUIRE( bitmap.Counting(false) == 4);
    REQUIRE( (bitmap.Counting(false) + bitmap.Counting(true)) == bitmap.Size());
}


TEST_CASE("Out put", "[BitMap]") {
    std::string bits {"1 0 1 0 1 0 1 0 1"};
    BitMap bitmap (bits);
    REQUIRE( bitmap.Size() ==  9);
    REQUIRE( bitmap.GetBitValue(0) ==  true);
    REQUIRE( bitmap.GetBitValue(1) ==  false);
    REQUIRE( bitmap.Counting(true) == 5);
    REQUIRE( bitmap.Counting(false) == 4);
    REQUIRE( (bitmap.Counting(false) + bitmap.Counting(true)) == bitmap.Size());
    std::ostringstream os;
    os << bitmap;
    std::cout <<"Out push >> " << os.str();
    REQUIRE( std::string(os.str()) == std::string("1 0 1 0 1 0 1 0 1 "));
    std::string newbit (" 1 1 1 1 1 1 1 1 1");
    std::istringstream ins (newbit);
    ins >> bitmap;
    REQUIRE( bitmap.Size() ==  9);
    REQUIRE( bitmap.GetBitValue(0) ==  true);
    REQUIRE( bitmap.GetBitValue(1) ==  true);
    REQUIRE( bitmap.Counting(true) == 9);
    REQUIRE( bitmap.Counting(false) == 0);
    REQUIRE( (bitmap.Counting(false) + bitmap.Counting(true)) == bitmap.Size());
}


