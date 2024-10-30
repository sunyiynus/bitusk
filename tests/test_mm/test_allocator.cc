#include "allocator.hpp"
#include "catch2/catch_all.hpp"

using namespace bitusk;

// 测试用例
TEST_CASE("Allocator default constructor", "[allocator]") {
    allocator<int> int_alloc;
    REQUIRE_NOTHROW(allocator<int>());
}

TEST_CASE("Allocator copy constructor", "[allocator]") {
    allocator<int> int_alloc;
    allocator<int> copied_alloc(int_alloc);
    REQUIRE_NOTHROW(copied_alloc);
}

TEST_CASE("Allocator allocate and deallocate", "[allocator]") {
    allocator<int> int_alloc;
    constexpr std::size_t n = 5;
    int* arr = int_alloc.allocate(n, 1);
    REQUIRE(arr != nullptr);

    for (std::size_t i = 0; i < n; ++i) {
        REQUIRE(arr[i] == 1);
    }

    int_alloc.deallocate(arr, n);  // 检查是否成功释放
    REQUIRE(true); // 如果没有异常抛出，测试通过
}

TEST_CASE("Custom operator new throws bad_alloc on failure", "[operator new]") {
    // 设置一个大的内存分配以模拟失败（具体的分配大小可以根据系统设置调整）
    std::size_t big_size = static_cast<std::size_t>(-1);
    REQUIRE_THROWS_AS(operator new(big_size), std::bad_alloc);
}

TEST_CASE("Allocator void specialization", "[allocator<void>]") {
    allocator<void> void_alloc;
    REQUIRE_NOTHROW(allocator<void>());
}

TEST_CASE("Allocator with complex data types", "[allocator<ComplexType>]") {
    struct ComplexType {
        int x;
        double y;
        ComplexType(int a, double b) : x(a), y(b) {}
        ComplexType() = default;
    };

    allocator<ComplexType> complex_alloc;
    constexpr std::size_t n = 3;
    ComplexType* complex_arr = complex_alloc.allocate(n, 12, 13.1);
    REQUIRE(complex_arr != nullptr);

    for (std::size_t i = 0; i < n; ++i) {
        REQUIRE(complex_arr[i].x == 12);
        REQUIRE(complex_arr[i].y == 13.1);
    }

    complex_alloc.deallocate(complex_arr, n);
    REQUIRE(true);  // 如果没有异常抛出，测试通过
    WARN(__FILE__);
}