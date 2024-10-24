#include <unistd.h>
#include "mem.h"
#include "types.h"
#include "list.h"


#ifdef PACK_STRUCT
#define PACK_ATTRIBUTE __attribute__((packed))
#endif

#define AREAN_POOL_SIZE 32

#define MAIN_BUFFER_SIZE (1024 * 512) // 512K
#define BUFFER_CHUCK_SIZE (1024)

#define UN_INIT_AREAN 0u
#define INITED_AREAN 1u

struct arean;

struct chunk_info {
    uint8_t dirty;
    uint8_t atomic;
    uint64_t bufferSize;
    struct arean* belong_arean_ptr;
    struct list_head list;
};


struct arean {
    uint8_t init_flag;
    uint8_t* buffer_ptr;
    size_t buffer_size;
    struct list_head used_list;
    struct list_head free_list;
    struct list_head arean_list;
};


uint8_t g_mainBuffer[MAIN_BUFFER_SIZE];
struct arean g_mainArean = {
    .init_flag = UN_INIT_AREAN,
    .buffer_ptr = g_mainBuffer,
    .buffer_size = sizeof(g_mainBuffer),
};

struct arean g_areanPool[AREAN_POOL_SIZE] = {0};
static int init_chuck_info(struct arean* );

uint32_t init_arean(struct arean* arean_ptr)
{
    INIT_LIST_HEAD(&(arean_ptr->used_list));
    INIT_LIST_HEAD(&(arean_ptr->free_list));
    INIT_LIST_HEAD(&(arean_ptr->arean_list));
    arean_ptr->init_flag = init_chuck_info(arean_ptr);
    arean_ptr->init_flag = INITED_AREAN;
    return INITED_AREAN;
}

int init_chuck_info(struct arean* arean_ptr)
{
    struct list_head* head = &(arean_ptr->free_list);
    uint8_t* ptr = arean_ptr->buffer_ptr;
    size_t bufferSize = arean_ptr->buffer_size;
    if (head == NULL || ptr == NULL) {
        return UN_INIT_AREAN;
    }
    uint8_t* endPtr = ptr + bufferSize;
    for (; ptr < endPtr;) {
        struct chunk_info* chuck_ptr = (struct chunk_info*)ptr;
        chuck_ptr->dirty = 0;
        chuck_ptr->bufferSize = BUFFER_CHUCK_SIZE;
        chuck_ptr->belong_arean_ptr = arean_ptr;
        list_head_insert(head, &(chuck_ptr->list));
        ptr += (sizeof(struct chunk_info) + chuck_ptr->bufferSize);
    }
    return INITED_AREAN;
}


struct chunk_info* find_suitable_chunk(const size_t size)
{
    struct list_head* indirect_chuck_ptr = &(g_mainArean.free_list);
    return NULL;
}

static inline void* intern_malloc(const size_t size)
{
    struct chunk_info* chuck_ptr = find_suitable_chunk(size);
    if (!chuck_ptr) {
        return NULL;
    }
    // remove from free list;
    struct list_head* node = list_head_remove(&(chuck_ptr->list));
    // insert into used list;
    struct arean* arean_ptr = chuck_ptr->belong_arean_ptr;
    list_head_insert(&(arean_ptr->used_list), node);
    return (void*)(chuck_ptr + sizeof(struct chunk_info));

}

static inline uint32_t intern_free(void* ptr)
{
    ptr -= sizeof(struct chunk_info);
    struct chunk_info* chuck_ptr = (struct chunk_info*)ptr;
    // remove from double list
    struct list_head* node = list_head_remove(&(chuck_ptr->list));
    // insert into free list
    struct arean* arean_ptr = chuck_ptr->belong_arean_ptr;
    list_head_insert(&(arean_ptr->free_list), node);
    return 0;
}


static inline void init_all_arean(void)
{
}

static inline uint32_t arean_memory_empty(struct arean* arean_ptr)
{

}


void* malloc(const size_t size)
{
    if (!g_mainArean.init_flag) {
        init_arean(&g_mainArean);
    }
}


int free(void* ptr)
{
    if (ptr != NULL) {
        return intern_free(ptr);
    }
    return -1;
}
