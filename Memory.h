#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <malloc.h>

extern void *memMalloc(size_t size);
extern void *memRealloc(void *ptr, size_t size);

#endif /*__MEMORY_H__*/
