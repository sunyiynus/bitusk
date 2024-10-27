#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "list.h"

TEST_CASE("INIT_LIST_HEAD initializes a list head correctly") {
    struct list_head head;
    INIT_LIST_HEAD(&head);

    REQUIRE(head.next == &head);
    REQUIRE(head.prev == &head);
}

TEST_CASE("list_head_insert inserts a new node correctly") {
    struct list_head head, node1;
    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&node1);

    list_head_insert(&head, &node1);

    REQUIRE(head.next == &node1);
    REQUIRE(node1.prev == &head);
    REQUIRE(node1.next == &head);
    REQUIRE(head.prev == &node1);
}

TEST_CASE("list_head_remove removes a node correctly") {
    struct list_head head, node1;
    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&node1);

    list_head_insert(&head, &node1);
    struct list_head* removed_node = list_head_remove(&node1);

    REQUIRE(removed_node == &node1);
    REQUIRE(head.next == &head);
    REQUIRE(head.prev == &head);
    REQUIRE(node1.next == NULL);
    REQUIRE(node1.prev == NULL);
}

TEST_CASE("list_head_remove_tail removes the last node correctly") {
    struct list_head head, node1, node2;
    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&node1);
    INIT_LIST_HEAD(&node2);

    list_head_insert(&head, &node1);
    list_head_insert(&node1, &node2);

    struct list_head* removed_tail = list_head_remove_tail(&head);

    REQUIRE(removed_tail == &node2);
    REQUIRE(head.next == &node1);
    REQUIRE(head.prev == &node1);
    REQUIRE(node1.next == &head);
    REQUIRE(node1.prev == &head);
}

TEST_CASE("list_head_remove_head removes the first node correctly") {
    struct list_head head, node1, node2;
    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&node1);
    INIT_LIST_HEAD(&node2);

    list_head_insert(&head, &node1);
    list_head_insert(&node1, &node2);

    struct list_head* removed_head = list_head_remove_head(&head);

    REQUIRE(removed_head == &node1);
    REQUIRE(head.next == &node2);
    REQUIRE(head.prev == &node2);
    REQUIRE(node2.next == &head);
    REQUIRE(node2.prev == &head);
}

TEST_CASE("list_head_empty returns true for an empty list") {
    struct list_head head;
    INIT_LIST_HEAD(&head);

    REQUIRE(list_head_empty(&head) == 1);
}

TEST_CASE("list_head_empty returns false for a non-empty list") {
    struct list_head head, node1;
    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&node1);

    list_head_insert(&head, &node1);

    REQUIRE(list_head_empty(&head) == 0);
}

TEST_CASE("list_length returns correct length for an empty list") {
    struct list_head head;
    INIT_LIST_HEAD(&head);

    REQUIRE(list_head_len(&head) == 0);
}

TEST_CASE("list_length returns correct length for a list with one node") {
    struct list_head head, node1;
    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&node1);

    list_head_insert(&head, &node1);

    REQUIRE(list_head_len(&head) == 1);
}

TEST_CASE("list_length returns correct length for a list with multiple nodes") {
    struct list_head head, node1, node2, node3;
    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&node1);
    INIT_LIST_HEAD(&node2);
    INIT_LIST_HEAD(&node3);

    list_head_insert(&head, &node1);
    list_head_insert(&node1, &node2);
    list_head_insert(&node2, &node3);

    REQUIRE(list_head_len(&head) == 3);
}


#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
static struct list_head new_node[600];

TEST_CASE("list_length handles more than 20 nodes with random insertions and deletions") {
    struct list_head head;
    INIT_LIST_HEAD(&head);

    // 初始化随机数种子
    std::srand(static_cast<unsigned int>(std::time(0)));

    // 插入 25 个节点
    struct list_head nodes[25];
    for (int i = 0; i < 25; ++i) {
        INIT_LIST_HEAD(&nodes[i]);
        list_head_insert(&head, &nodes[i]);
    }

    REQUIRE(list_head_len(&head) == 25);  // 确保插入25个节点后，长度为25

    // 随机删除和插入操作
    int insertions = 0;
    int deletions = 0;
    size_t new_node_idx = 0;
    for (int i = 0; i < 60000; ++i) {
        int operation = rand() % 2;  // 生成0或1，决定插入或删除操作

        if (operation == 0 && insertions < 25) {
            // 插入新的节点
            INIT_LIST_HEAD(&new_node[new_node_idx]);
            list_head_insert(&head, &new_node[new_node_idx]);
            insertions++;
            new_node_idx++;
        } else if (operation == 1 && deletions < insertions) {
            // 随机删除现有节点
            struct list_head* node_to_remove = head.next;
            list_head_remove(node_to_remove);
            deletions++;
        }

        // 每次操作后检查长度是否合理
        REQUIRE(list_head_len(&head) == (25 + insertions - deletions));
    }

    // 最后验证链表长度
    int final_length = 25 + insertions - deletions;
    REQUIRE(list_head_len(&head) == final_length);
}



// 测试用的简单结构体，包含 list_head 节点
struct test_node {
    int value;
    struct list_head list;
};

// 初始化测试链表和节点
void setup_test_list(struct list_head* head, struct test_node nodes[], int num_nodes) {
    INIT_LIST_HEAD(head);
    for (int i = 0; i < num_nodes; ++i) {
        INIT_LIST_HEAD(&nodes[i].list);  // 初始化每个节点的 list_head
        nodes[i].value = i;               // 给每个节点赋值
        list_head_insert(head, &nodes[i].list);  // 插入到链表
    }
}

TEST_CASE("list_head_for_each iterates over each node in the list") {
    struct list_head head;
    const int num_nodes = 5;
    struct test_node nodes[num_nodes];

    setup_test_list(&head, nodes, num_nodes);

    // 遍历链表并检查每个节点的值
    struct list_head* pos;
    int i = 0;
    list_head_for_each(pos, &head) {
        struct test_node* node = list_entry(pos, struct test_node, list);
        REQUIRE(node->value == nodes[num_nodes - i - 1].value);  // 检查节点值
        i++;
    }

    REQUIRE(i == num_nodes);  // 确保遍历了 num_nodes 个节点
}

TEST_CASE("list_head_for_each handles empty list") {
    struct list_head head;
    INIT_LIST_HEAD(&head);

    // 空链表遍历，应该不进入循环
    struct list_head* pos;
    int counter = 0;
    list_head_for_each(pos, &head) {
        counter++;
    }

    REQUIRE(counter == 0);  // 确保遍历计数为0
}