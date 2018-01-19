#include "town.h"
#include "strategies.h"
#include "network.h"
#include "args.h"
#include "neighbours.h"
#include "utils.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


/* thread data for accept_neighbours */
struct accept_neighbours_data {
    struct town_descriptor *town;
    sem_t *pop_lock;
    short port;
};

/* thread wrapper for accept_neighbours */
void *accept_neighbours_process(void *data) {
    struct accept_neighbours_data *arg = data;
    accept_neighbours(arg->town, arg->pop_lock, arg->port);
    // process is not supposed to stop
    exit(EXIT_FAILURE);
}

/* neighbour outgoing connection thread data */
struct neighbour_data {
    struct town_descriptor *town;
    struct neighbour *neighbour;
    char *destination;
    short port;
};

/* neighbour outgoing connection thread wrapper */
void *neighbour_thread(void *data) {
    struct neighbour_data *arg = data;
    send_migrants(arg->town, arg->neighbour, arg->destination, arg->port);
    return NULL;
}


/* population simulation thread data */
struct population_data {
    struct town_descriptor *town;
    sem_t *pop_lock;
    sem_t *next;
    sem_t *done;
    struct neighbour *neighbours;
    int n_neighbours;
    double prob_mig;
};

/* thread wrapper for population_parallel */
void *population_process(void *data) {
    struct population_data *arg = data;
    population_parallel(arg->town, arg->pop_lock, arg->next, arg->done,
                        arg->prob_mig, arg->neighbours, arg->n_neighbours);
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
    if(socket < 0) {
        perror("open_listen_socket");
        exit(EXIT_FAILURE);
    }

    int fd = wait_for_client(socket);
    if(socket < 0) {
        perror("wait_for_client");
        exit(EXIT_FAILURE);
    }

    // set global monitoring FILE
    *mon = fdopen(fd, "w");

    char c;
    // wait
    while(read(fd, &c, 1) > 0);
    exit(EXIT_SUCCESS);
}


int main(int argc, char **argv) {
    // initialization
    srand(time(NULL));

    // parse arguments
    struct argp_data option_data = parse_arguments(argc, argv);
    struct town_descriptor *town = option_data.town;
    short mon_port = option_data.mon_port;
    short in_port = option_data.in_port;
    char **neighbour_names = option_data.neighbours;
    short *neighbour_ports = option_data.neighbour_ports;
    int n_neighbours = option_data.n_neighbours;
    double prob_mig = option_data.prob_mig;

    // global semaphores
    sem_t pop_lock, next, done;
    sem_init(&pop_lock, 0, 1);
    sem_init(&next, 0, 0);
    sem_init(&done, 0, 0);

    // launch monitoring connection thread
    FILE *mon = NULL;
    struct mon_data mon_dt = {&mon, mon_port};
    if(mon_port >= 0) {
        pthread_t mon_t;
        pthread_create(&mon_t, NULL, monitor_com, &mon_dt);
        pthread_detach(mon_t);
        // wait for connection
        while(!mon);
    }

    // launch neighbours incomming connection thread
    struct accept_neighbours_data accept_dt = {town, &pop_lock, in_port};
    if(in_port >= 0) {
        pthread_t neighbours_t;
        pthread_create(&neighbours_t, NULL, accept_neighbours_process, &accept_dt);
        pthread_detach(neighbours_t);
    }

    // launch neighbours outgoing connection threads
    struct neighbour *neighbours = NULL;
    struct neighbour_data *neighbours_data = NULL;
    if(n_neighbours > 0) {
        neighbours = malloc(n_neighbours * sizeof(struct neighbour));
        if(!neighbours) fatal_perror("malloc");

        neighbours_data = malloc(n_neighbours * sizeof(struct neighbour_data));
        if(!neighbours) fatal_perror("malloc");

        for(int i=0; i<n_neighbours; i++) {
            neighbours[i].migrants = malloc(town->n_strategies * sizeof(long));
            if(!neighbours[i].migrants) fatal_perror("malloc");
            neighbours[i].allowed = malloc(town->n_strategies * sizeof(char));
            if(!neighbours[i].allowed) fatal_perror("malloc");

            for(int j=0; j<town->n_strategies; j++) {
                neighbours[i].migrants[j] = 0;
                neighbours[i].allowed[j] = 0;
            }

            sem_init(&neighbours[i].mig_lock, 0, 1);
            sem_init(&neighbours[i].send, 0, 0);

            neighbours_data[i].town = town;
            neighbours_data[i].neighbour = &neighbours[i];
            neighbours_data[i].destination = neighbour_names[i];
            neighbours_data[i].port = neighbour_ports[i];

            pthread_t thread;
            pthread_create(&thread, NULL, neighbour_thread, &neighbours_data[i]);
            pthread_detach(thread);
        }
    }

    // launch population thread
    struct population_data data;
    data.town = town;
    data.pop_lock = &pop_lock;
    data.next = &next;
    data.done = &done;
    data.neighbours = neighbours;
    data.n_neighbours = n_neighbours;
    data.prob_mig = prob_mig;

    pthread_t pop_t;
    pthread_create(&pop_t, NULL, population_process, &data);
    pthread_detach(pop_t);

    for(int step=0;;step++) {
        sem_post(&next);
        sem_wait(&done);
        sem_wait(&pop_lock);
        if(mon) {
            for(int i=0; i<town->n_strategies; i++) {
                if(town->allowed[i]) {
                    fprintf(mon, "%s %ld %ld ",  town->strategies[i].very_short_name,
                            town->population[i], town->scores[i]);
                } else {
                    fprintf(mon, "0 0 ");
                }
            }
            fflush(mon);
        } else {
            printf("step %d\n", step);
            for(int i=0; i<town->n_strategies; i++) {
                if(!town->allowed[i]) continue;
                printf("%s\t%ld\t%ld\n", town->strategies[i].short_name,
                       town->population[i], town->scores[i]);
            }
            printf("\n");
        }
        sem_post(&pop_lock);
        sleep(1);
    }

    return 0;
}
