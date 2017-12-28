#ifndef POPULATION_H_INCLUDED
#define POPULATION_H_INCLUDED

#include "strategies.h"

/** Simulate populations interaction.
 * Uses integer population sizes.
 * @param strategies the array of strategies of the various populations.
 * @param n_strategies the number of populations.
 * @param n the number of steps to simulate.
 * @param rewards the array of reward values.
 * @param initial_pop the array of initial population sizes.
 * @param result an already allocated array of size `n * n_strategies`.
 *   result[t][i] is filled with the size of population i at step t.
 * @return 0 on success, -1 on error. */
int populations(struct strategy_entry *strategies, int n_strategies,
                int n, int rewards[2][2],
                long *initial_pop, long **result);

/** Simulate populations interaction.
 * Uses population proportions.
 * @param strategies the array of strategies of the various populations.
 * @param n_strategies the number of populations.
 * @param n the number of steps to simulate.
 * @param rewards the array of reward values.
 * @param initial_prop the array of initial population proportions.
 * @param result an already allocated array of size `n * n_strategies`.
 *   result[t][i] is filled with the proportion of population i at step t.
 * @return 0 on success, -1 on error. */
int proportions(struct strategy_entry *strategies, int n_strategies,
                int n, int rewards[2][2],
                double *initial_prop, double **result);

#endif /* end of include guard: POPULATION_H_INCLUDED */
