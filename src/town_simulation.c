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

int step;

void *population_process(void *data) {
    struct population_data *arg = (struct population_data *)data;
    population_parallel(arg->town, arg->pop_lock, arg->next, arg->done);
    return NULL;
}

// just quit as soon as a message is sent, or connection is closed.
// it's stupid, but it works
void *monitor_com(void *mon) {
    int socket = open_listen_socket(4000);
    if(socket < 0) exit(EXIT_FAILURE);

    int fd = wait_for_client(socket);
    if(fd < 0) exit(EXIT_FAILURE);

    *(FILE **)mon = fdopen(fd, "w");

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

    FILE *mon = NULL;

    pthread_t pop_t, mon_t;
    pthread_create(&pop_t, NULL, population_process, (void*)&data);
    pthread_create(&mon_t, NULL, monitor_com, (void*)&mon);


    for(step=0;;step++) {
        if(mon) {
            fprintf(mon, "step %d\n", step);
            for(int i=0; i<town->n_strategies; i++) {
                if(!town->allowed[i]) continue;
                fprintf(mon, "%s\t%ld\n", town->strategies[i].short_name, town->population[i]);
            }
            fprintf(mon, "\n");
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
