#ifndef TOWN_H_INCLUDED
#define TOWN_H_INCLUDED

#include "strategies.h"
#include "semaphore.h"

struct town_descriptor {
    int n_strategies;
    struct strategy_entry *strategies;
    char *allowed;
    int (*rewards)[2][2];
    long *population;
};

void fatal_error(char *error);
void fatal_perror(char *msg);

void population_parallel(struct town_descriptor *town, sem_t *pop_lock,
                         sem_t *next, sem_t *done);


#endif /* end of include guard: TOWN_H_INCLUDED */
