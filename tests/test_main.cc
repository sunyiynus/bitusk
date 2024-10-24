#include "catch2/catch_all.hpp"



static int add_test(int a, int b)
{
    return a + b;
}



TEST_CASE("add_test case1", "[ad_test_case1]")
{
    REQUIRE( add_test(1, 1) ==  2);
}