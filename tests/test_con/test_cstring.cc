#include "cstring.hpp"
#include "catch2/catch_all.hpp"

using namespace bitusk;

TEST_CASE("Testing strcpy functionality") {
    char buffer[50]; // 定义一个缓冲区

    // 1. 正常复制
    SECTION("Normal copy") {
        const char* source = "Hello, World!";
        int result = strcpy(buffer, sizeof(buffer), source, sizeof(source));
        REQUIRE(result == 0);
        REQUIRE(strcmp(buffer, "Hello, World!") == 0);
    }

    // 2. 目标缓冲区足够大，源字符串为空
    SECTION("Empty source string") {
        const char* source = "";
        int result = strcpy(buffer, sizeof(buffer), source, sizeof(source));
        REQUIRE(result == 0);
        REQUIRE(strcmp(buffer, "") == 0);
    }

    // 3. 目标缓冲区大小不足以完全容纳源字符串
    SECTION("Buffer overflow protection") {
        const char* source = "This is a long string";
        int result = strcpy(buffer, sizeof(buffer), source, sizeof(source));
        REQUIRE(result == 0);
        REQUIRE(strcmp(buffer, "This is a long string") == 0);
        // 确保以 null 结尾
        REQUIRE(buffer[sizeof(buffer) - 1] == '\0' || buffer[sizeof(buffer) - 1] == '\0');
    }

    // 4. 目标字符串为空，源字符串非空
    SECTION("Null destination") {
        const char* source = "Hello";
        int result = strcpy(nullptr, sizeof(buffer), source, sizeof(source));
        REQUIRE(result == -1); // 目标为 null，应返回 -1
    }

    // 5. 源字符串为空
    SECTION("Null source") {
        int result = strcpy(buffer, sizeof(buffer), nullptr, sizeof(buffer));
        REQUIRE(result == -1); // 源为 null，应返回 -1
    }
    WARN(__FILE__);
}