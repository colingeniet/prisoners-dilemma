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
        perror("malloc");
        ret = -1;
        goto end;
    }
    scores = multi_malloc(3, (size_t)n_strategies, (size_t)n_strategies,
                              (size_t)n * sizeof(int));
    if(!scores) {
        perror("malloc");
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
        if(sum) {
            if(proportion(result[step], points, n_strategies, total_pop)) {
                perror("malloc");
                ret = -1;
                goto end;
            }
        } else {
            // if the total score is 0, do nothing !
            // this can happen if the population is empty,
            // or with some weird reward values
            for(int i=0; i<n_strategies; i++) {
                result[step][i] = result[step-1][i];
            }
        }
    }

    end:
    multi_free(scores, 3, (size_t)n_strategies, (size_t)n_strategies);
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
        perror("malloc");
        ret = -1;
        goto end;
    }
    scores = multi_malloc(3, (size_t)n_strategies, (size_t)n_strategies,
                              (size_t)n * sizeof(int));
    if(!scores) {
        perror("malloc");
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

        // if the total score is 0, do nothing !
        // this can happen with some weird reward values
        if(sum != 0) {
            for(int i=0; i<n_strategies; i++) {
                result[step][i] = points[i] / sum;
            }
        } else {
            for(int i=0; i<n_strategies; i++) {
                result[step][i] = result[step-1][i];
            }
        }
    }

    end:
    multi_free(scores, 3, (size_t)n_strategies, (size_t)n_strategies);
    free(points);
    return ret;
}
