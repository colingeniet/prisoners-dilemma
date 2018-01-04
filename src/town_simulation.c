#include "town.h"
#include "strategies.h"
#include "network.h"
#include "args.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct population_data {
    struct town_descriptor *town;
    sem_t *pop_lock;
    sem_t *next;
    sem_t *done;
};

void *population_process(void *data) {
    struct population_data *arg = (struct population_data *)data;
    population_parallel(arg->town, arg->pop_lock, arg->next, arg->done);
    return NULL;
}

// just quit as soon as a message is sent, or connection is closed.
void *monitor_com(void *data) {
    int socket = open_listen_socket(4000);
    int fd = wait_for_client(socket);

    char c;
    read(fd, &c, 1);
    exit(0);
}

int main(int argc, char **argv) {
    struct town_descriptor *town = parse_arguments(argc, argv);

    sem_t pop_lock, next, done;
    sem_init(&pop_lock, 0, 1);
    sem_init(&next, 0, 0);
    sem_init(&done, 0, 0);

    struct population_data data;
    data.town = town;
    data.pop_lock = &pop_lock;
    data.next = &next;
    data.done = &done;

    pthread_t pop_t, mon_t;
    pthread_create(&pop_t, NULL, population_process, (void*)&data);
    pthread_create(&mon_t, NULL, monitor_com, NULL);

    for(int step=0;;step++) {
        printf("step %d\n", step);
        for(int i=0; i<town->n_strategies; i++) {
            if(!town->allowed[i]) continue;
            printf("%s\t%ld\n", town->strategies[i].short_name, town->population[i]);
        }
        printf("\n");
        sem_post(&next);
        sleep(1);
        sem_wait(&done);
    }

    return 0;
}
