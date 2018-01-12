#include "neighbours.h"
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


/* handle the connection from a neighbour, and modify population as needed. */
void handle_neighbour(struct town_descriptor *town, sem_t *pop_lock, int sock) {
    FILE *com = fdopen(sock, "r+");

    // send allowed strategies
    for(int strat=0; strat<town->n_strategies; strat++) {
        if(town->allowed[strat]) {
            fprintf(com, "%s ", town->strategies[strat].very_short_name);
        }
    }
    fprintf(com, "end ");

    // wait for migrants
    for(;;) {
        long pop;
        char strat_name[10];
        if(fscanf(com, " %s %ld", strat_name, &pop) != 2) continue;

        for(int strat=0; strat<town->n_strategies; strat++) {
            if(!town->allowed[strat]) continue;
            if(!strcmp(strat_name, town->strategies[strat].very_short_name)) {
                sem_wait(pop_lock);
                town->population[strat] += pop;
                sem_post(pop_lock);
                break;
            }
        }
    }
}

struct handle_neighbour_data {
    struct town_descriptor *town;
    sem_t *pop_lock;
    int sock;
};

void *handle_neighbour_thread(void *_data) {
    struct handle_neighbour_data *data = _data;
    handle_neighbour(data->town, data->pop_lock, data->sock);
    return NULL;
}

/* wait for neighbours connections on a port, and launch a new handle_neighbour
 * thread for each connection. */
int accept_neighbours(struct town_descriptor *town, sem_t *pop_lock, short port) {
    int sock = open_listen_socket(port);
    if(sock < 0) return -1;

    for(;;) {
        int new_client = wait_for_client(sock);
        if(new_client < 0) continue;

        struct handle_neighbour_data *data;
        data = malloc(sizeof(struct handle_neighbour_data));
        if(!data) {
            perror("malloc");
            return -1;
        }
        data->town = town;
        data->pop_lock = pop_lock;
        data->sock = new_client;

        pthread_t thread;
        pthread_create(&thread, NULL, handle_neighbour_thread, data);
        pthread_detach(thread);
    }
}
