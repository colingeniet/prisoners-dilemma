#include "strategies.h"
#include <stdlib.h>
#include <stdio.h>

action strat_nice(int n_played, action *hist, int n_coop) {
    return COOP;
}

action strat_bad(int n_played, action *hist, int n_coop) {
    return DEFECTS;
}

action strat_give_give(int n_played, action *hist, int n_coop) {
    return n_played < 1 || hist[n_played-1];
}

action strat_distrustful(int n_played, action *hist, int n_coop) {
    return n_played >= 1 && hist[n_played-1];
}

action strat_give_give_hard(int n_played, action *hist, int n_coop) {
    return (n_played < 1 || hist[n_played-1])
        && (n_played < 2|| hist[n_played-2]);
}

action strat_spiteful(int n_played, action *hist, int n_coop) {
    return n_coop == n_played;
}

action strat_periodic_nice(int n_played, action *hist, int n_coop) {
    return n_played%3 != 2;
}

action strat_periodic_bad(int n_played, action *hist, int n_coop)  {
    return n_played%3 == 2;
}

action strat_majority_nice(int n_played, action *hist, int n_coop) {
    return 2*n_coop >= n_played;
}

action strat_majority_bad(int n_played, action *hist, int n_coop) {
    return 2*n_coop > n_played;
}

action strat_poll(int n_played, action *hist, int n_coop) {
    if(n_played < 3) return !(n_played%3);
    else if(hist[2] && hist[3]) return DEFECTS;
    else return hist[n_played-1];
}


struct strategy_entry strategies[N_STRATEGIES] = {
   {strat_nice, "gentille", "gent", "g"},
   {strat_bad, "méchante", "méch", "m"},
   {strat_give_give, "donnant-donnant", "d-d", "dd"},
   {strat_distrustful, "méfiante", "méf", "mf"},
   {strat_give_give_hard, "donnant-donnant-dur", "d-d dur", "ddd"},
   {strat_spiteful, "rancunière", "ranc", "r"},
   {strat_periodic_nice, "périodique gentille", "p gent", "pg"},
   {strat_periodic_bad, "périodique méchante", "p méch", "pm"},
   {strat_majority_nice, "majorité mou", "maj mou", "mm"},
   {strat_majority_bad, "majorité dur", "maj dur", "md"},
   {strat_poll, "sondeur", "sond", "s"}
};

int default_rewards[2][2] = {{1, 5}, {0, 3}};


int iterate_dilemma(strategy player1, strategy player2, int n,
                    int rewards[2][2], int *res1, int *res2) {
    int reward1=0, reward2=0;
    int n_coop1=0, n_coop2=0;
    int result;
    action *hist1=NULL, *hist2=NULL;
    hist1 = (action*)malloc(n*sizeof(action));
    hist2 = (action*)malloc(n*sizeof(action));
    if(!hist1 || !hist2) {
        perror("malloc");
        result=-1;
        goto end;
    }

    for(int i=0; i<n; i++) {
        /* actions are 1=cooperates, 0=defects */
        action act1 = player1(i, hist2, n_coop2);
        action act2 = player2(i, hist1, n_coop1);
        n_coop1 += act1; n_coop2 += act2;
        hist1[i] = act1; hist2[i] = act2;
        reward1 += rewards[act1][act2];
        reward2 += rewards[act2][act1];
    }

    if(res1) *res1 = reward1;
    if(res2) *res2 = reward2;
    result = 0;

    end:
    free(hist1);
    free(hist2);
    return result;
}


int try_strategies(struct strategy_entry *strategies, int n_strategies, int n,
                   int rewards[2][2], int **results) {
    if(!results) {
        fprintf(stderr, "try_strategies : results array not allocated.\n");
        return -1;
    }

    for(int i=0; i<n_strategies; i++) {
        if(!results[i]) {
            fprintf(stderr, "try_strategies : results array not allocated.\n");
            return -1;
        }
        for(int j=0; j<n_strategies; j++) {
            if(iterate_dilemma(strategies[i].strat, strategies[j].strat, n,
                               rewards, &results[i][j], NULL)) {
                results[i][j] = 0;
            }
        }
    }
    return 0;
}


int iterate_dilemma_detail(strategy player1, strategy player2, int n,
                           int rewards[2][2], int *res1, int *res2) {
    int n_coop1=0, n_coop2=0;
    int result;
    action *hist1=NULL, *hist2=NULL;
    hist1 = (action*)malloc(n*sizeof(action));
    hist2 = (action*)malloc(n*sizeof(action));
    if(!hist1 || !hist2) {
        perror("malloc");
        result=-1;
        goto end;
    }

    for(int i=0; i<n; i++) {
        /* actions are 1=cooperates, 0=defects */
        action act1 = player1(i, hist2, n_coop2);
        action act2 = player2(i, hist1, n_coop1);
        n_coop1 += act1; n_coop2 += act2;
        hist1[i] = act1; hist2[i] = act2;
        if(res1) res1[i] = rewards[act1][act2];
        if(res2) res2[i] = rewards[act2][act1];
    }

    result = 0;

    end:
    free(hist1);
    free(hist2);
    return result;
}


int try_strategies_detail(struct strategy_entry *strategies, int n_strategies,
                          int n, int rewards[2][2], int ***results) {
    if(!results) {
        fprintf(stderr, "try_strategies_detail : results array not allocated.\n");
        return -1;
    }

    for(int i=0; i<n_strategies; i++) {
        if(!results[i]) {
            fprintf(stderr, "try_strategies_detail : results array not allocated.\n");
            return -1;
        }
        for(int j=0; j<n_strategies; j++) {
            iterate_dilemma_detail(strategies[i].strat, strategies[j].strat, n,
                                   rewards, results[i][j], NULL);
        }
    }
    return 0;
}
