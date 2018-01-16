#include "population.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>


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
