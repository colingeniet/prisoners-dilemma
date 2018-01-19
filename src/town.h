#ifndef TOWN_H_INCLUDED
#define TOWN_H_INCLUDED

#include "strategies.h"
#include <semaphore.h>

/** Describes the state of a town. */
struct town_descriptor {
    struct strategy_entry *strategies;  /**< The array of strategies. */
    int n_strategies;                   /**< The number of strategies. */
    /** A boolean array. Indicates if a strategie is allowed. */
    char *allowed;
    int (*rewards)[2][2];               /**< The reward values. */
    long *population;                   /**< The population for each strategy. */
    long *scores;                       /**< The score for each strategy. */
};

struct neighbour {
    char *allowed;
    long *migrants;
    sem_t mig_lock;
    sem_t send;
};

/** Simulates population evolution in a multithreaded environment.
 * @param town the initial state of the town. Its `population` field
 *   will be modified as the population evolves.
 * @param pop_lock a mutex semaphore for `town`. Ignored if NULL.
 * @param next a semaphore. sem_wait(next) is called before each step
 *   of the simulation. Ignored if NULL.
 * @param done a semaphore. sem_post(done) is called after each step
 *   of the simulation. Ignored if NULL.
 * `next` and `done` can be used to coordinate the simulation with
 * other threads. */
void population_parallel(struct town_descriptor *town, sem_t *pop_lock,
                         sem_t *next, sem_t *done, double prob_mig,
                         struct neighbour *neighbours, int n_neighbours);


#endif /* end of include guard: TOWN_H_INCLUDED */
