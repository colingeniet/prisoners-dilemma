#include "network.h"
#include "strategies.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N_HOSTS 1


struct mon_thread_data {
    FILE *mon_stream;
    long *pop;
    char *modified;
    sem_t *pop_sem;
};

void *update_pop(void *_data) {
    struct mon_thread_data *data = _data;
    long pop_tmp[N_STRATEGIES];

    while(1) {
        for(int strat=0; strat<N_STRATEGIES; strat++) {
            fscanf(data->mon_stream, " %ld", &pop_tmp[strat]);
        }
        sem_wait(data->pop_sem);
        for(int strat=0; strat<N_STRATEGIES; strat++) {
            data->pop[strat] = pop_tmp[strat];
        }
        *data->modified = 1;
        sem_post(data->pop_sem);
    }
}

int main(int argc, char **argv) {
    char host_name[] = "00.dptinfo.ens-cachan.fr";
    char command[500];

    int fd[N_HOSTS];
    FILE *streams[N_HOSTS];
    long populations[N_HOSTS][N_STRATEGIES] = {0};
    struct mon_thread_data data[N_HOSTS];
    pthread_t threads[N_HOSTS];
    char modified;
    sem_t pop_sem;

    sem_init(&pop_sem, 0, N_HOSTS);

    for(int host=0; host<N_HOSTS; host++) {
        sprintf(host_name, "%.2d.dptinfo.ens-cachan.fr", host+1);
        sprintf(command, "%s -p 100", argv[1]);
        if(!fork()) {
            execlp("ssh", "ssh", host_name, command, NULL);
            perror("monitor");
            exit(EXIT_FAILURE);
        }

        sprintf(command, "%.2d.dptinfo.ens-cachan.fr", host+1);
        int ret = -1;
        while(ret < 0) {
            sleep(1);
            ret = connect_to_server(command, 4000);
        }
        fd[host] = ret;
        streams[host] = fdopen(fd[host], "r");
        setlinebuf(streams[host]);

        data[host].mon_stream = streams[host];
        data[host].pop = populations[host];
        data[host].modified = &modified;
        data[host].pop_sem = &pop_sem;
        pthread_create(&threads[host], NULL, update_pop, &data[host]);
    }

    while(1) {
        while(!modified);

        for(int i=0; i<N_HOSTS; i++) {
            sem_wait(&pop_sem);
        }

        for(int strat=0; strat<N_STRATEGIES; strat++) {
            printf("\t%s", strategies[strat].short_name);
        }
        printf("\n");
        for(int host=0; host<N_HOSTS; host++) {
            printf("%.2d", host);
            for(int strat=0; strat<N_STRATEGIES; strat++) {
                printf("\t%ld", populations[host][strat]);
            }
            printf("\n");
        }
        printf("\n");

        for(int i=0; i<N_HOSTS; i++) {
            sem_post(&pop_sem);
        }
    }
}
