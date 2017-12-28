#include "utils.h"
#include <stdarg.h>

void* multi_malloc(size_t dimensions, size_t *sizes) {
    if(dimensions <= 0) return NULL;
    if(!sizes) return NULL;

    void *mem = malloc((*sizes) * sizeof(void*));
    if(!mem) return NULL;
    for(size_t i=0; i<*sizes; i++) {
        ((void**)mem)[i] = multi_malloc(dimensions-1, sizes+1);
        if(!((void**)mem)[i]) {
            multi_free(dimensions, sizes, mem);
            return NULL;
        }
    }
    return mem;
}

void multi_free(size_t dimensions, size_t *sizes, void* mem) {
    if(!mem) return;

    if(dimensions <= 0) return;
    else if(dimensions == 1) free(mem);
    else {
        for(size_t i=0; i<*sizes; i++) {
            multi_free(dimensions-1, sizes+1, ((void**)mem)[i]);
        }
        free(mem);
    }
}
