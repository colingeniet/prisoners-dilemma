#include "town.h"
#include "strategies.h"
#include "network.h"
#include "args.h"
#include "neighbours.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* population simulation thread data */
struct population_data {
    struct town_descriptor *town;
    sem_t *pop_lock;
    sem_t *next;
    sem_t *done;
};

/* thread wrapper for population_parallel */
void *population_process(void *data) {
    struct population_data *arg = (struct population_data *)data;
    population_parallel(arg->town, arg->pop_lock, arg->next, arg->done);
    // process is not supposed to stop
    exit(EXIT_FAILURE);
}

/* thread data for accept_neighbours */
struct accept_neighbours_data {
    struct town_descriptor *town;
    sem_t *pop_lock;
    short port;
};

/* thread wrapper for accept_neighbours */
void *accept_neighbours_process(void *data) {
    struct accept_neighbours_data *arg = (struct accept_neighbours_data *)data;
    accept_neighbours(arg->town, arg->pop_lock, arg->port);
    // process is not supposed to stop
    exit(EXIT_FAILURE);
}


/* monitor connection thread data */
struct mon_data {
    FILE **mon;
    short port;
};

/* monitor connection thread :
 * establish connection, and quit if connection is closed. */
void *monitor_com(void *_data) {
    struct mon_data *data = _data;
    FILE **mon = data->mon;
    short port = data->port;

    if(port < 0) return NULL;

    // create connection
    int socket = open_listen_socket(port);
    if(socket < 0) exit(EXIT_FAILURE);

    int fd = wait_for_client(socket);
    if(socket < 0) exit(EXIT_FAILURE);

    // set global monitoring FILE
    *mon = fdopen(fd, "w");

    char c;
    // wait
    while(read(fd, &c, 1) > 0);
    exit(EXIT_SUCCESS);
}


int main(int argc, char **argv) {
    struct argp_data option_data = parse_arguments(argc, argv);
    struct town_descriptor *town = option_data.town;
    short mon_port = option_data.mon_port;
    short in_port = option_data.in_port;

    sem_t pop_lock, next, done;
    sem_init(&pop_lock, 0, 1);
    sem_init(&next, 0, 0);
    sem_init(&done, 0, 0);

    struct population_data data;
    data.town = town;
    data.pop_lock = &pop_lock;
    data.next = &next;
    data.done = &done;

    // launch population thread
    pthread_t pop_t;
    pthread_create(&pop_t, NULL, population_process, (void*)&data);
    pthread_detach(pop_t);

    // launch neighbours incomming connection thread
    struct accept_neighbours_data accept_dt = {town, &pop_lock, in_port};
    if(in_port >= 0) {
        pthread_t neighbours_t;
        pthread_create(&neighbours_t, NULL, accept_neighbours_process, (void*)&accept_dt);
        pthread_detach(neighbours_t);
    }

    // launch monitoring connection thread
    FILE *mon = NULL;
    struct mon_data mon_dt = {&mon, mon_port};
    if(mon_port >= 0) {
        pthread_t mon_t;
        pthread_create(&mon_t, NULL, monitor_com, (void*)&mon_dt);
        pthread_detach(mon_t);
        // wait for connection
        while(!mon);
    }



    for(int step=0;;step++) {
        if(mon) {
            for(int i=0; i<town->n_strategies; i++) {
                if(town->allowed[i]) {
                    fprintf(mon, "%ld ", town->population[i]);
                } else {
                    fprintf(mon, "0 ");
                }
            }
            fflush(mon);
        } else {
            printf("step %d\n", step);
            for(int i=0; i<town->n_strategies; i++) {
                if(!town->allowed[i]) continue;
                printf("%s\t%ld\n", town->strategies[i].short_name, town->population[i]);
            }
            printf("\n");
        }
        sem_post(&next);
        sleep(1);
        sem_wait(&done);
    }

    return 0;
}
