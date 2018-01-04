#include "population.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>


int populations(struct strategy_entry *strategies, int n_strategies,
                int n, int rewards[2][2],
                long *initial_pop, long **result) {
    int ***scores;
    int ret = 0;
    long total_pop;
    long *points;
    if(!result) {
        fprintf(stderr, "proportions_detail : result array not allocated.\n");
        ret = -1;
        goto end;
    }

    points = malloc(n_strategies * sizeof(long));
    if(!points) {
        perror("proportions_detail");
        ret = -1;
        goto end;
    }
    scores = multi_mallocv(3, (size_t)n_strategies, (size_t)n_strategies,
                              (size_t)n * sizeof(int));
    if(!scores) {
        perror("proportions_detail");
        ret = -1;
        goto end;
    }
    try_strategies_detail(strategies, n_strategies, n, rewards, scores);

    total_pop = 0;
    for(int i=0; i<n_strategies; i++) {
        total_pop += initial_pop[i];
    }

    /* copy initial population */
    for(int i=0; i<n_strategies; i++) {
        result[0][i] = initial_pop[i];
    }
    /* run simulation */
    for(int step=1; step<n; step++) {
        /* compute the points earned by each strategy */
        for(int i=0; i<n_strategies; i++) {
            points[i] = 0;
            for(int j=0; j<n_strategies; j++) {
                points[i] += result[step-1][j] * scores[i][j][step];
            }
            /* a person does not confront itself */
            points[i] -= scores[i][i][step];

            points[i] *= result[step-1][i];
        }

        /* compute the proportions of each strategy at next step */
        long sum = 0;
        for(int i=0; i<n_strategies; i++) {
            sum += points[i];
        }

        for(int i=0; i<n_strategies; i++) {
            /* TO DO : ensure the total population does not change
             * the sum of the various populations is not exactly constant,
             * but at least this makes sure it does not change too much */
            result[step][i] = (total_pop * points[i]) / sum;
        }
    }

    end:
    multi_freev(scores, 3, (size_t)n_strategies, (size_t)n_strategies);
    free(points);
    return ret;
}

int proportions(struct strategy_entry *strategies, int n_strategies,
                int n, int rewards[2][2],
                double *initial_prop, double **result) {
    int ***scores;
    int ret = 0;
    double *points;
    if(!result) {
        fprintf(stderr, "proportions_detail : result array not allocated.\n");
        ret = -1;
        goto end;
    }

    points = malloc(n_strategies * sizeof(double));
    if(!points) {
        perror("proportions_detail");
        ret = -1;
        goto end;
    }
    scores = multi_mallocv(3, (size_t)n_strategies, (size_t)n_strategies,
                              (size_t)n * sizeof(int));
    if(!scores) {
        perror("proportions_detail");
        ret = -1;
        goto end;
    }
    try_strategies_detail(strategies, n_strategies, n, rewards, scores);

    /* copy initial population */
    for(int i=0; i<n_strategies; i++) {
        result[0][i] = initial_prop[i];
    }
    /* run simulation */
    for(int step=1; step<n; step++) {
        /* compute the points earned by each strategy */
        for(int i=0; i<n_strategies; i++) {
            points[i] = 0;
            for(int j=0; j<n_strategies; j++) {
                points[i] += result[step-1][j] * scores[i][j][step];
            }
            points[i] *= result[step-1][i];
        }

        /* compute the proportions of each strategy at next step */
        double sum = 0;
        for(int i=0; i<n_strategies; i++) {
            sum += points[i];
        }

        for(int i=0; i<n_strategies; i++) {
            result[step][i] = points[i] / sum;
        }
    }

    end:
    multi_freev(scores, 3, (size_t)n_strategies, (size_t)n_strategies);
    free(points);
    return ret;
}
