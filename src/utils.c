#include "utils.h"

void* multi_malloc(size_t dimensions, size_t *sizes) {
    if(!sizes) return NULL;
    if(dimensions <= 0) return NULL;
    else if(dimensions == 1) return malloc(*sizes);
    else {
        void *mem = malloc((*sizes) * sizeof(void*));
        if(!mem) return NULL;
        for(size_t i=0; i<*sizes; i++) {
            ((void**)mem)[i] = multi_malloc(dimensions-1, sizes+1);
            if(!((void**)mem)[i]) {
                multi_free(mem, dimensions, sizes);
                return NULL;
            }
        }
        return mem;
    }
}


void* multi_malloc_var(size_t dimensions, va_list sizes) {
    if(dimensions <= 0) {
        va_end(sizes);
        return NULL;
    } else if(dimensions == 1) {
        void* mem = malloc(va_arg(sizes, size_t));
        va_end(sizes);
        return mem;
    } else {
        va_list bck_sizes;
        va_copy(bck_sizes, sizes);

        size_t size = va_arg(sizes, size_t);
        void *mem = malloc(size * sizeof(void*));
        if(!mem) {
            va_end(sizes);
            return NULL;
        }
        for(size_t i=0; i<size; i++) {
            va_list new_sizes;
            va_copy(new_sizes, sizes);
            ((void**)mem)[i] = multi_malloc_var(dimensions-1, new_sizes);
            if(!((void**)mem)[i]) {
                multi_free_var(mem, dimensions, bck_sizes);
                va_end(sizes);
                return NULL;
            }
        }
        va_end(bck_sizes);
        va_end(sizes);
        return mem;
    }
}

void* multi_mallocv(size_t dimensions, ...) {
    va_list vl;
    va_start(vl, dimensions);
    return multi_malloc_var(dimensions, vl);
}



void multi_free(void *mem, size_t dimensions, size_t *sizes) {
    if(!mem) return;

    if(dimensions <= 0) return;
    else if(dimensions == 1) free(mem);
    else {
        for(size_t i=0; i<*sizes; i++) {
            multi_free(((void**)mem)[i], dimensions-1, sizes+1);
        }
        free(mem);
    }
}


void multi_free_var(void *mem, size_t dimensions, va_list sizes) {
    if(!mem) {
        va_end(sizes);
        return;
    }

    if(dimensions <= 0) {
        va_end(sizes);
    } else if(dimensions == 1) {
        va_end(sizes);
        free(mem);
    } else {
        size_t size = va_arg(sizes, size_t);
        for(size_t i=0; i<size; i++) {
            va_list new_sizes;
            va_copy(new_sizes, sizes);
            multi_free_var(((void**)mem)[i], dimensions-1, new_sizes);
        }
        va_end(sizes);
        free(mem);
    }
}

void multi_freev(void *mem, size_t dimensions, ...) {
    va_list vl;
    va_start(vl, dimensions);
    multi_free_var(mem, dimensions, vl);
}
