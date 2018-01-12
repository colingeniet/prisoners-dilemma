#ifndef NEIGHBOURS_H_INCLUDED
#define NEIGHBOURS_H_INCLUDED

#include "town.h"
#include <semaphore.h>

/** Accept connections form neighbours on `port`
 * @param town the town descriptor, modified when migrants arrive.
 * @param pop_lock a town descriptor mutex lock.
 * @param port the port on which connections are accepted.
 * @result -1 on error. Does not return if nothing fails. */
int accept_neighbours(struct town_descriptor *town, sem_t *pop_lock, short port);

#endif /* end of include guard: NEIGHBOURS_H_INCLUDED */
