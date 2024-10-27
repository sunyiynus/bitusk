
// /*
#include "catch2/catch_all.hpp"
#include "list.h" // 引入链表实现
#include "mem.c" // 假设以上代码文件为 arean.hpp

TEST_CASE("Initialization of arean") {
    // 测试初始化单个 arean
    struct arean testArean;
    testArean.init_flag = UN_INIT_AREAN;
    testArean.buffer_ptr = new uint8_t[MAIN_BUFFER_SIZE];
    testArean.buffer_size = MAIN_BUFFER_SIZE;

    REQUIRE(init_arean(&testArean) == INITED_AREAN);
    REQUIRE(testArean.init_flag == INITED_AREAN);

    // 确保free_list不为空，并且所有chunk正确初始化
    struct list_head* pos = nullptr;
    int chunk_count = 0;
    list_head_for_each(pos, &testArean.free_list) {
        struct chunk_info* chunk = list_entry(pos, struct chunk_info, list);
        REQUIRE(chunk->dirty == 0);
        REQUIRE(chunk->bufferSize == BUFFER_CHUCK_SIZE);
        REQUIRE(chunk->belong_arean_ptr == &testArean);
        chunk_count++;
    }
    REQUIRE(chunk_count > 0); // 确保至少有一个 chunk 被初始化

    delete[] testArean.buffer_ptr; // 清理分配的内存
}

TEST_CASE("Allocate and free memory from arean") {
    init_arean(&g_mainArean);  // 初始化主 arean

    // 测试分配一个小块内存
    void* ptr1 = smalloc(128);
    REQUIRE(ptr1 != nullptr);

    // 检查是否从 free_list 移动到 used_list
    struct list_head* pos = nullptr;
    bool found = false;
    list_head_for_each(pos, &g_mainArean.used_list) {
        struct chunk_info* chunk = list_entry(pos, struct chunk_info, list);
        if ((void*)(chunk + 1) == ptr1) {
            found = true;
            break;
        }
    }
    REQUIRE(found);

    // 释放内存并检查是否返回到 free_list
    REQUIRE(sfree(ptr1) == 0);
    found = false;
    list_head_for_each(pos, &g_mainArean.free_list) {
        struct chunk_info* chunk = list_entry(pos, struct chunk_info, list);
        if ((void*)(chunk + 1) == ptr1) {
            found = true;
            break;
        }
    }
    REQUIRE(found);
}

TEST_CASE("Multiple allocations and frees") {
    init_arean(&g_mainArean);  // 初始化主 arean

    // 分配多块内存
    void* ptrs[10];
    for (int i = 0; i < 10; ++i) {
        ptrs[i] = smalloc(128);
        REQUIRE(ptrs[i] != nullptr);
    }

    // 检查是否都在 used_list 中
    struct list_head* pos = nullptr;
    int used_count = 0;
    list_head_for_each(pos, &g_mainArean.used_list) {
        used_count++;
    }
    REQUIRE(used_count == 10);

    // 随机释放其中一些内存块
    REQUIRE(sfree(ptrs[2]) == 0);
    REQUIRE(sfree(ptrs[5]) == 0);
    REQUIRE(sfree(ptrs[7]) == 0);

    // 检查 used_list 和 free_list 的节点数量是否一致
    used_count = 0;
    int free_count = 0;
    list_head_for_each(pos, &g_mainArean.used_list) {
        used_count++;
    }
    list_head_for_each(pos, &g_mainArean.free_list) {
        free_count++;
    }
    REQUIRE(used_count == 7);
    REQUIRE(free_count > 0);
}

void* ptrs[700];
TEST_CASE("Multiple allocations and frees - Stress Test with 100,000 iterations") {
    // init_arean(&g_mainArean);  // 初始化主 arean

    // 重复进行10万次内存分配和释放
    const int num_iterations = 100000;
    const int alloc_size = 128;
    const int alloc_count = 700;
    
    for (int j = 0; j < num_iterations; ++j) {
        // 分配多块内存
        
        for (int i = 0; i < alloc_count; ++i) {
            ptrs[i] = smalloc(alloc_size);
            REQUIRE(ptrs[i] != nullptr);
        }

        // 检查是否都在 used_list 中
        struct list_head* pos = nullptr;
        int used_count = 0;
        list_head_for_each(pos, &g_mainArean.used_list) {
            used_count++;
        }
        REQUIRE(used_count == alloc_count);

        // 随机释放其中一些内存块
        int free_indices[] = {2, 5, 7};  // 固定释放的索引
        for (int idx : free_indices) {
            REQUIRE(sfree(ptrs[idx]) == 0);
        }

        // 检查 used_list 和 free_list 的节点数量是否一致
        used_count = 0;
        int free_count = 0;
        list_head_for_each(pos, &g_mainArean.used_list) {
            used_count++;
        }
        list_head_for_each(pos, &g_mainArean.free_list) {
            free_count++;
        }
        REQUIRE(used_count == alloc_count - 3);
        REQUIRE(free_count > 0);
    }
}
// */