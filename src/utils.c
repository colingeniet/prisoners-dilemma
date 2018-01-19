#include "utils.h"
#include <stdio.h>


void* multi_mallocv(size_t dimensions, va_list sizes) {
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
            ((void**)mem)[i] = multi_mallocv(dimensions-1, new_sizes);
            if(!((void**)mem)[i]) {
                multi_freev(mem, dimensions, bck_sizes);
                va_end(sizes);
                return NULL;
            }
        }
        va_end(bck_sizes);
        va_end(sizes);
        return mem;
    }
}

void* multi_malloc(size_t dimensions, ...) {
    va_list vl;
    va_start(vl, dimensions);
    return multi_mallocv(dimensions, vl);
}



void multi_freev(void *mem, size_t dimensions, va_list sizes) {
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
            multi_freev(((void**)mem)[i], dimensions-1, new_sizes);
        }
        va_end(sizes);
        free(mem);
    }
}

void multi_free(void *mem, size_t dimensions, ...) {
    va_list vl;
    va_start(vl, dimensions);
    multi_freev(mem, dimensions, vl);
}




void fatal_error(char *msg) {
    fprintf(stderr, "%s.\n", msg);
    exit(EXIT_FAILURE);
}

void fatal_perror(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}



int proportion(long *result, long *coefs, int n, long total) {
    long *rests = malloc(n*sizeof(long));
    if(!rests) return -1;
    int *perm = malloc(n*sizeof(int));
    if(!perm) {
        free(rests);
        return -1;
    }

    // calculate the sum of coefficients
    long total_coef = 0;
    for(int i=0; i<n; i++) {
        total_coef += coefs[i];
    }
    if(total_coef == 0) return -1;

    long diff = total;
    for(int i=0; i<n; i++) {
        // approximate results : just use floor
        result[i] = (coefs[i]*total)/total_coef;
        // diff is the difference between the current sum and the expected total
        diff -= result[i];
        // calculate the remainders
        rests[i] = (coefs[i]*total)%total_coef;
        // and initialize the permutation indices array
        perm[i] = i;
    }
    // sort the remainders (biggest first)
    for(int i=1; i<n; i++) {
        int x = perm[i];
        int j = i;
        while(j>0 && rests[perm[j-1]] < rests[x]) {
            perm[j] = perm[j-1];
            j--;
        }
        perm[j] = x;
    }
    // increase the results with biggest remainders to reach total
    for(;diff-->0;) result[perm[diff]]++;   // yes it works

    free(rests); free(perm);
    return 0;
}
