#include <stdio.h>
#include <stdlib.h>
#include "strategies.h"

int main(int argc, char **argv) {
    int ret, n;
    int **results = NULL;
    if(argc != 2) {
        fprintf(stderr, "usage : dileme1 <n>\n");
        ret = 1;
        goto end;
    }
    n = atoi(argv[1]);

    results = (int**)malloc(N_STRATEGIES*sizeof(int*));
    if(!results) {
        perror("main : ");
        ret = 2;
        goto end;
    }
    for(int i=0; i<N_STRATEGIES; i++) {
        results[i] = NULL;
        results[i] = (int*)malloc(N_STRATEGIES*sizeof(int));
        if(!results[i]) {
            perror("main : ");
            ret = 2;
            goto end;
        }
    }

    try_strategies(strategies, N_STRATEGIES, n, default_rewards, results);

    end:
    if(results) {
        for(int i=0; i<N_STRATEGIES; i++) free(results[i]);
    }
    free(results);
    return ret;
}
