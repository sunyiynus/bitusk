#ifndef MEM_H

#include "types.h"

void* smalloc(const size_t size);
int sfree(void* ptr);

#endif // MEM_H