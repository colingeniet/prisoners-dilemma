#include "neighbours.h"
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


/* handle the connection from a neighbour, and modify population as needed. */
void handle_neighbour(struct town_descriptor *town, sem_t *pop_lock, int sock) {
    FILE *com = fdopen(sock, "r+");
    if(!com) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

    // send allowed strategies
    for(int strat=0; strat<town->n_strategies; strat++) {
        if(town->allowed[strat]) {
            fprintf(com, "%s ", town->strategies[strat].very_short_name);
        }
    }
    fprintf(com, "end ");
    fflush(com);

    // wait for migrants
    for(;;) {
        long pop;
        char strat_name[10];
        int ret = fscanf(com, " %s %ld", strat_name, &pop);
        if(ret == EOF) return;      // connection closed, or error
        else if(ret != 2) continue; // invalid input

        for(int strat=0; strat<town->n_strategies; strat++) {
            if(!town->allowed[strat]) continue;
            if(!strcmp(strat_name, town->strategies[strat].very_short_name)) {
                sem_wait(pop_lock);
                town->population[strat] += pop;
                sem_post(pop_lock);
                break; // ignore any other strategy with the same name
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



int send_migrants(struct town_descriptor *town, long *migrants, sem_t *mig_lock,
                  char *destination, short port, char *allowed, sem_t *send) {
    int sock = connect_to_server(destination, port);
    if(sock < 0) {
        return -1;
    }

    FILE *com = fdopen(sock, "r+");
    if(!com) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

    // receive allowed strategies
    char strat_name[10];
    for(;;) {
        int ret = fscanf(com, " %9s", strat_name);
        if(ret == EOF) return -1;
        else if(ret != 1) continue; // invalid input

        if(!strcmp(strat_name, "end")) break;

        for(int strat=0; strat<town->n_strategies; strat++) {
            if(!strcmp(strat_name, town->strategies[strat].very_short_name)) {
                allowed[strat] = 1;
                continue; // ignore any other strategy with the same name
            }
        }
    }

    // send migrants
    for(;;) {
        sem_wait(send); // wait for send signal
        sem_wait(mig_lock);
        for(int strat=0; strat<town->n_strategies; strat++) {
            if(!allowed[strat] || !town->allowed[strat]) continue;

            if(migrants[strat] > 0) {
                fprintf(com, "%s %ld ", town->strategies[strat].very_short_name,
                        migrants[strat]);
                migrants[strat] = 0;
            }
        }
        sem_post(mig_lock);
        fflush(com);
    }
}
