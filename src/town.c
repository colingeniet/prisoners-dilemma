#include "town.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



void population_parallel(struct town_descriptor *town, sem_t *pop_lock,
                         sem_t *next, sem_t *done, double prob_mig,
                         struct neighbour *neighbours, int n_neighbours) {
    int steps_alloc = 16;

    // initialize arrays
    action ***actions = NULL;
    actions = multi_malloc(2, town->n_strategies, town->n_strategies * sizeof(action*));
    if(!actions) fatal_perror("malloc");

    int **n_coop = NULL;
    n_coop = multi_malloc(2, town->n_strategies, town->n_strategies * sizeof(int));
    if(!n_coop) fatal_perror("malloc");

    long *scores = NULL;
    scores = malloc(town->n_strategies * sizeof(long));
    if(!scores) fatal_perror("malloc");

    long *migrants = NULL;
    if(n_neighbours > 0) {
        migrants = malloc(n_neighbours * sizeof(long));
        if(!migrants) fatal_perror("malloc");
    }

    int *accept_neighbours = NULL;
    if(n_neighbours > 0) {
        accept_neighbours = malloc(n_neighbours * sizeof(int));
        if(!accept_neighbours) fatal_perror("malloc");
    }

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
        sem_wait(next);

        // reallocate memory if needed
        if(step >= steps_alloc) {
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
            if(proportion(town->population, scores, town->n_strategies, total_pop)) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
        }

        // do migrations
        for(int strat=0; strat<town->n_strategies; strat++) {
            int n_accept_neighbours = 0;
            // create the array of accepting neighbours
            for(int i=0; i<n_neighbours; i++) {
                if(neighbours[i].allowed[strat]) {
                    accept_neighbours[n_accept_neighbours++] = i;
                }
            }
            // no accepting town
            if(!n_accept_neighbours) continue;

            // migrants is the number of migrants to each neighbour, for
            // this strategy
            for(int i=0; i<n_neighbours; i++) {
                migrants[i] = 0;
            }
            for(long i=0; i<town->population[strat]; i++) {
                // decide to migrate or not
                if((double) rand() / RAND_MAX >= prob_mig) continue;

                // choose a destination town randomly
                int dest = rand() % n_accept_neighbours;
                migrants[accept_neighbours[dest]]++;
            }

            // copy results to migration thread
            for(int i=0; i<n_neighbours; i++) {
                if(migrants[i] > 0) {
                    town->population[strat] -= migrants[i];
                    sem_wait(&neighbours[i].mig_lock);
                    neighbours[i].migrants[strat] += migrants[i];
                    sem_post(&neighbours[i].mig_lock);
                }
            }
        }

        sem_post(pop_lock);
        // send end signals
        sem_post(done);
        for(int i=0; i<n_neighbours; i++) {
            sem_post(&neighbours[i].send);
        }
    }
}
