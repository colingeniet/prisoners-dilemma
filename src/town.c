#include "town.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void fatal_error(char *error) {
    fprintf(stderr, "%s.\n", error);
    exit(EXIT_FAILURE);
}

void fatal_perror(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}


void population_parallel(struct town_descriptor *town, sem_t *pop_lock,
                         sem_t *next, sem_t *done)
{
    int steps_alloc = 128;

    // initialize arrays
    action ***actions = NULL;
    actions = multi_mallocv(2, town->n_strategies, town->n_strategies * sizeof(action*));
    if(!actions) fatal_perror("town");

    int **n_coop = NULL;
    n_coop = multi_mallocv(2, town->n_strategies, town->n_strategies * sizeof(int));
    if(!n_coop) fatal_perror("town");

    long *scores = NULL;
    scores = malloc(town->n_strategies * sizeof(long));
    if(!scores) fatal_perror("town");

    // only allocate memory for allowed startegies !
    for(int i=0; i<town->n_strategies; i++) {
        for(int j=0; j<town->n_strategies; j++) {
            n_coop[i][j] = 0;
            actions[i][j] = NULL;
            if(town->allowed[i] && town->allowed[j]) {
                actions[i][j] = malloc(steps_alloc * sizeof(action));
                if(!actions[i][j]) fatal_perror("town");
            }
        }
    }

    // main loop
    for(int step=0;;step++) {
        if(next) sem_wait(next);

        // reallocate memory if needed
        if(step > steps_alloc) {
            steps_alloc *= 2;
            for(int i=0; i<town->n_strategies; i++) {
                if(!town->allowed[i]) continue;
                for(int j=0; j<town->n_strategies; j++) {
                    if(!town->allowed[j]) continue;
                    action *mem = realloc(actions[i][j], steps_alloc * sizeof(action));
                    if(!mem) fatal_perror("town");
                    else actions[i][j] = mem;
                }
            }
        }

        // simulate strategies
        for(int i=0; i<town->n_strategies; i++) {
            if(!town->allowed[i]) continue;
            for(int j=0; j<town->n_strategies; j++) {
                if(!town->allowed[j]) continue;
                actions[i][j][step] = town->strategies[i].strat(step, actions[j][i], n_coop[j][i]);
            }
        }
        // update stats
        for(int i=0; i<town->n_strategies; i++) {
            if(!town->allowed[i]) continue;
            for(int j=0; j<town->n_strategies; j++) {
                if(!town->allowed[j]) continue;
                n_coop[i][j] += actions[i][j][step];
            }
        }

        // calculate scores
        // population values need to be stable
        if(pop_lock) sem_wait(pop_lock);
        for(int i=0; i<town->n_strategies; i++) {
            if(!town->allowed[i]) continue;
            scores[i] = 0;
            for(int j=0; j<town->n_strategies; j++) {
                if(!town->allowed[j]) continue;
                // score of i vs j at current step
                long score = (*town->rewards)[actions[i][j][step]]
                                             [actions[j][i][step]];
                if(i != j) scores[i] += score * town->population[j];
                else scores[i] += score * (town->population[i] - 1);
            }
            scores[i] *= town->population[i];
        }

        // update population
        long total_pop = 0, total_score = 0;
        for(int i=0; i<town->n_strategies; i++) {
            if(town->allowed[i]) {
                total_pop += town->population[i];
                total_score += scores[i];
            }
        }

        // if the total score is 0, do nothing !
        // this can happen if the population is empty,
        // or with some weird reward values
        if(total_score) {
            for(int i=0; i<town->n_strategies; i++) {
                if(town->allowed[i]) {
                    town->population[i] = (total_pop * scores[i]) / total_score;
                }
            }
        }

        if(pop_lock) sem_post(pop_lock);
        if(done) sem_post(done);
    }
}
