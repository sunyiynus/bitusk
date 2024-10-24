#ifndef MEM_H

#include "types.h"

void* malloc(const size_t size);
int free(void* ptr);

#endif // MEM_H