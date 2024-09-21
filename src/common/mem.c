#include "mem.h"
#include "types.h"
#include <unistd.h>

struct arean {
    
};

struct chunk_info {
    uint8_t dirty;
    uint8_t atomic;
    struct chunk_info *prev;
    struct chunk_info *next;
};








void* malloc(const size_t size)
{
    
}