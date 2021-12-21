#include <boost/smart_ptr/shared_ptr.hpp>
#include <sstream>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "datacache.hpp"

//#include "bitfield_test.cpp"

TEST_CASE("test node init", "[Struct Node]") {
    Node tnode(1,2);
    REQUIRE(tnode.index == 1);
    REQUIRE(tnode.offset == 2);
    REQUIRE(tnode.in_use == true);
    REQUIRE(tnode.write_read == true);
    REQUIRE(tnode.is_writed == true);
    REQUIRE(tnode.access_count == 0);
    SECTION("Function test.."){
        ++tnode;
        REQUIRE(tnode.access_count == 1);   
        tnode++;
        REQUIRE(tnode.access_count == 2);   
    }

    SECTION("bool operator") {
        REQUIRE( (tnode ? true:false) == true);
    }
}


TEST_CASE("test node default", "[Struct Node]") {
    Node tnode;
    REQUIRE(tnode.index == 0);
    REQUIRE(tnode.offset == 0);
    REQUIRE(tnode.in_use == false);
    REQUIRE(tnode.write_read == true);
    REQUIRE(tnode.is_writed == true);
    REQUIRE(tnode.access_count == 0);
    SECTION("Function test.."){
        ++tnode;
        REQUIRE(tnode.access_count == 1);   
        tnode++;
        REQUIRE(tnode.access_count == 2);   
    }

    SECTION("bool operator") {
        REQUIRE( (tnode ? true:false) == false);
        //REQUIRE( tnode == true);
    }
}




TEST_CASE("cache testing...SwapOut", "[class CacheV1]") {
    CacheV1 cache;
    FilesManager fmanager;
    cache.SetFileManager( &fmanager);
    std::ostringstream content;
    content << "aaaaaaaaaaaaaaaaaaaa";
    for(int i = 0; i < 80; ++i ) {
        Node node(i, 1);
        content << i;
        ustring c (reinterpret_cast<const unsigned char*>(std::string(content.str()).c_str()));
        node.buffer = c;
        cache.Refer(node);
        auto nodeptr = cache.Get(i,1);
        REQUIRE( nodeptr->buffer == c);
        if( i > 64 ) {
            REQUIRE( cache.Size() == 64);
        }
    }
}





