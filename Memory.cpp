#include "Memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define die(...) \
{ \
	printf(__VA_ARGS__); \
	exit(1); \
}

void *memMalloc(size_t size)
{
	void *ret = malloc(size);
	if (ret == NULL)
		die("**** rSON Fatal ****\nCould not allocate enough memory!\n**** rSON Fatal ****");
	memset(ret, 0, size);
	return ret;
}

void *memRealloc(void *ptr, size_t size)
{
	void *ret = realloc(ptr, size);
	if (ret == NULL)
		die("**** rSON Fatal ****\nCould not reallocate memory - out of memory!\n**** rSON Fatal ****");
	return ret;
}
