#include "network.h"
#include "strategies.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


/* monitoring connection thread data */
struct mon_thread_data {
    char *host_name;
    short port;
    long *pop;
    char *modified;
    sem_t *pop_sem;
};

/* monitoring connection : wait for data from monitored town,
 * and update the population array */
void *update_pop(void *_data) {
    struct mon_thread_data *data = _data;
    /* the population array is only updated when all the values for
     * one step have been received.
     * This array is used to temporarely store them. */
    long pop_tmp[N_STRATEGIES];

    // try to connect
    int fd = -1;
    while(fd < 0) {
        sleep(1);
        fd = connect_to_server(data->host_name, data->port);
    }
    FILE *stream = fdopen(fd, "r");

    while(1) {
        // wait for data
        for(int strat=0; strat<N_STRATEGIES; strat++) {
            if(fscanf(stream, " %ld", &pop_tmp[strat]) == EOF) {
                fprintf(stderr, "connection with %s closed.\n", data->host_name);
                return NULL;
            }
        }
        // update
        sem_wait(data->pop_sem);
        for(int strat=0; strat<N_STRATEGIES; strat++) {
            data->pop[strat] = pop_tmp[strat];
        }
        *data->modified = 1;
        sem_post(data->pop_sem);
    }
}

int main(int argc, char **argv) {
    int n_hosts = argc-1;
    char **host_names = argv+1;

    long **populations;
    struct mon_thread_data *data;
    pthread_t *threads;
    char modified = 1;
    sem_t pop_sem;

    populations = multi_malloc(2, n_hosts, N_STRATEGIES*sizeof(long));
    if(!populations) {
        perror("malloc");
        goto fail;
    }
    data = malloc(n_hosts * sizeof(struct mon_thread_data));
    if(!data) {
        perror("malloc");
        goto fail;
    }
    threads = malloc(n_hosts * sizeof(pthread_t));
    if(!threads) {
        perror("malloc");
        goto fail;
    }

    sem_init(&pop_sem, 0, n_hosts);

    // launch connection threads
    for(int host=0; host<n_hosts; host++) {
        // split the host name
        char *delim = strrchr(host_names[host], ':');
        if(!delim) {
            fprintf(stderr, "invalid host name : %s\n", host_names[host]);
            fprintf(stderr, "excepected syntax : HOST:PORT\n");
            goto fail;
        }
        *delim = '\0';
        char *port_str = delim+1;
        char *end;
        short port = strtol(port_str, &end, 10);
        if(*end) {
            fprintf(stderr, "invalid port : %s\n", port_str);
            goto fail;
        }

        data[host].host_name = host_names[host];
        data[host].port = port;
        data[host].pop = populations[host];
        data[host].modified = &modified;
        data[host].pop_sem = &pop_sem;
        pthread_create(&threads[host], NULL, update_pop, &data[host]);
        pthread_detach(threads[host]);
    }

    // display results
    while(1) {
        sleep(1); // don't update too often
        while(!modified);

        for(int i=0; i<n_hosts; i++) {
            sem_wait(&pop_sem);
        }

        printf("\t\t");
        for(int strat=0; strat<N_STRATEGIES; strat++) {
            printf("%s\t", strategies[strat].short_name);
        }
        printf("\n");
        for(int host=0; host<n_hosts; host++) {
            if(strlen(host_names[host]) <= 14) {
                printf("%-14s\t", host_names[host]);
            } else {
                printf("%-11.11s...\t", host_names[host]);
            }
            for(int strat=0; strat<N_STRATEGIES; strat++) {
                printf("%ld\t", populations[host][strat]);
            }
            printf("\n");
        }
        printf("\n");

        modified = 0;

        for(int i=0; i<n_hosts; i++) {
            sem_post(&pop_sem);
        }
    }

    fail:
    fprintf(stderr, "Fatal error in monitor - Warning, monitored processes may \
not be terminated automatically.\n");
    exit(EXIT_FAILURE);
}
