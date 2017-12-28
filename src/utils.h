#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdlib.h>

void* multi_malloc(size_t dimensions, size_t *sizes);

void multi_free(size_t dimensions, size_t *sizes, void* mem);

#endif /* end of include guard: UTILS_H_INCLUDED */
