#ifndef POPULATION_H_INCLUDED
#define POPULATION_H_INCLUDED

#include "strategies.h"

int populations(struct strategy_entry *strategies, int n_strategies,
                int n, int rewards[2][2],
                long *initial_pop, long **result);

int proportions(struct strategy_entry *strategies, int n_strategies,
                int n, int rewards[2][2],
                double *initial_prop, double **result);

#endif /* end of include guard: POPULATION_H_INCLUDED */
