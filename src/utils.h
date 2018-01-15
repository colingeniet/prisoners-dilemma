#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdlib.h>
#include <stdarg.h>

/** Allocate a multidimentionnal array.
 * @param dimensions the number of dimensions requested.
 * @param ... `dimensions` arguments of type `size_t`.
 * @return a multidimentionnal array of dimensions given by
     the additional arguments, or NULL if an error occured. */
void* multi_malloc(size_t dimensions, ...);

/** Allocate a multidimentionnal array.
 * @param dimensions the number of dimensions requested.
 * @param sizes an argument list of type `size_t`, of length `dimensions`.
 * @return a multidimentionnal array of dimensions given by `sizes`,
     or NULL if an error occured. */
void* multi_mallocv(size_t dimensions, va_list sizes);


/** Allocate a multidimentionnal array.
 * @param dimensions the number of dimensions requested.
 * @param ... `dimensions - 1` arguments of type `size_t`.
 *   The last dimesion may be ommited */
void multi_free(void *mem, size_t dimensions, ...);

/** Free a multidimentionnal array.
 * @param mem the array to free.
 * @param dimensions the number of dimensions of `mem`.
 * @param sizes the dimensions of `mem` as an argument list
 *   of length `dimensions - 1`. The last dimension may be ommited. */
void multi_freev(void *mem, size_t dimensions, va_list sizes);

#endif /* end of include guard: UTILS_H_INCLUDED */
