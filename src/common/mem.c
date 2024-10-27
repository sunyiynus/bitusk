#include <unistd.h>
#include "mem.h"
#include "types.h"
#include "list.h"


#ifdef PACK_STRUCT
#define PACK_ATTRIBUTE __attribute__((packed))
#endif

#define AREAN_POOL_SIZE 32

#define MAIN_BUFFER_SIZE (1024 * 512) // 512K
#define AREAN_BUFFER_SIZE (1024 * 512) // 512K
#define BUFFER_CHUCK_SIZE (1024)
#define GLOBAL_BUFFER_POOL_SIZE  (1024 * 512 * 32) // 4MB
#define LOG_QUEUE_SIZE (1024)


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


enum error_code {
    AREAN_NOT_ENOUGH,
    AREAN_INIT_FAILURE,
};

static uint8_t g_mainBuffer[MAIN_BUFFER_SIZE];
static struct arean g_mainArean = {
    .init_flag = UN_INIT_AREAN,
    .buffer_ptr = g_mainBuffer,
    .buffer_size = sizeof(g_mainBuffer),
};

static struct arean g_areanPool[AREAN_POOL_SIZE] = {0};
static uint8_t g_bufferPools[GLOBAL_BUFFER_POOL_SIZE] = {0};
static uint32_t g_bufferPoolsWaterLine = 0;

static uint16_t g_errorCode = 0;


struct log_cache {
    uint8_t g_logQueue[LOG_QUEUE_SIZE];


};



static int init_chuck_info(struct arean* );

static inline uint32_t init_arean(struct arean* arean_ptr)
{
    INIT_LIST_HEAD(&(arean_ptr->used_list));
    INIT_LIST_HEAD(&(arean_ptr->free_list));
    INIT_LIST_HEAD(&(arean_ptr->arean_list));
    arean_ptr->init_flag = init_chuck_info(arean_ptr);
    arean_ptr->init_flag = INITED_AREAN;
    return INITED_AREAN;
}

static inline int init_chuck_info(struct arean* arean_ptr)
{
    struct list_head* head = &(arean_ptr->free_list);
    uint8_t* ptr = arean_ptr->buffer_ptr;
    size_t bufferSize = arean_ptr->buffer_size;
    if (head == PTR_NULL || ptr == PTR_NULL) {
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

static inline struct arean* get_arean()
{
    struct arean* arean_ptr = PTR_NULL;
#ifdef BRK_ENABLE

#else
    for (size_t i = 0; i < N_ELEMENTS(g_areanPool); ++i) {
        if (!g_areanPool[i].init_flag) {
            arean_ptr = &(g_areanPool[i]);
            
            break;
        }
    }
    arean_ptr->buffer_ptr = &g_bufferPools[g_bufferPoolsWaterLine];
    g_bufferPoolsWaterLine+= AREAN_BUFFER_SIZE;
    arean_ptr->buffer_size = AREAN_BUFFER_SIZE;

#endif // BRK_ENABLE
    if (UN_INIT_AREAN == init_arean(arean_ptr)) {
        // LOG
    }
    return arean_ptr;
}


static inline struct arean* find_suitable_arean(void)
{
    struct list_head* free_list = &(g_mainArean.free_list);
    if (!list_head_empty(free_list)) {
        return &g_mainArean;
    }
    struct list_head* arean_head = &(g_mainArean.arean_list);
    struct list_head* pos;
    struct arean* arean_ptr = PTR_NULL;
    list_head_for_each(pos, arean_head) {
        arean_ptr = list_entry(pos,struct arean, arean_list);
        if (arean_ptr->init_flag && (!list_head_empty(&arean_ptr->free_list))) {
            break;
        }
    }
    if (!arean_ptr) {
        arean_ptr = get_arean();
        list_head_insert(&(g_mainArean.arean_list), &(arean_ptr->arean_list));
    }
    return arean_ptr;
}


static inline struct chunk_info* find_suitable_chunk(const size_t size)
{
    struct arean* arean_ptr = find_suitable_arean();
    struct list_head* free_node = arean_ptr->free_list.next;
    return list_entry(free_node, struct chunk_info, list);
}



static inline void* intern_malloc(const size_t size)
{
    struct chunk_info* chuck_ptr = find_suitable_chunk(size);
    if (!chuck_ptr) {
        return PTR_NULL;
    }
    // remove from free list;
    struct list_head* node = list_head_remove(&(chuck_ptr->list));
    // insert into used list;
    struct arean* arean_ptr = chuck_ptr->belong_arean_ptr;
    list_head_insert(&(arean_ptr->used_list), node);
    return (void*)(chuck_ptr + 1);

}

static inline uint32_t intern_free(void* ptr)
{
    struct chunk_info* chuck_ptr = (struct chunk_info*)ptr;
    chuck_ptr--;
    // remove from double list
    struct list_head* node = list_head_remove(&(chuck_ptr->list));
    // insert into free list
    struct arean* arean_ptr = chuck_ptr->belong_arean_ptr;
    list_head_insert(&(arean_ptr->free_list), node);
    return 0;
}


void* smalloc(const size_t size)
{
    if (!g_mainArean.init_flag) {
        init_arean(&g_mainArean);
    }
    return intern_malloc(size);
}


int sfree(void* ptr)
{
    if (ptr != PTR_NULL) {
        return intern_free(ptr);
    }
    return -1;
}
