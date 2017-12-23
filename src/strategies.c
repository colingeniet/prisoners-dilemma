#include "strategies.h"
#include <stdlib.h>
#include <stdio.h>

action nice(int n_played, action *hist, int n_coop) {
    return COOP;
}

action bad(int n_played, action *hist, int n_coop) {
    return DEFECTS;
}

action give_give(int n_played, action *hist, int n_coop) {
    return n_played < 1 || hist[n_played-1];
}

action distrustful(int n_played, action *hist, int n_coop) {
    return n_played >= 1 && hist[n_played-1];
}

action give_give_hard(int n_played, action *hist, int n_coop) {
    return (n_played < 1 || hist[n_played-1])
        && (n_played < 2|| hist[n_played-2]);
}

action spiteful(int n_played, action *hist, int n_coop) {
    return n_coop == n_played;
}

action period_nice(int n_played, action *hist, int n_coop) {
    return n_played%3 != 2;
}

action period_bad(int n_played, action *hist, int n_coop)  {
    return n_played%3 == 2;
}

action majority_nice(int n_played, action *hist, int n_coop) {
    return 2*n_coop >= n_played;
}

action majority_bad(int n_played, action *hist, int n_coop) {
    return 2*n_coop > n_played;
}

action poll(int n_played, action *hist, int n_coop) {
    if(n_played < 3) return !(n_played%3);
    else if(hist[2] && hist[3]) return DEFECTS;
    else return hist[n_played-1];
}


struct strategy_entry strategies[N_STRATEGIES] = {
   {nice, "gentille", "gent", "g"},
   {bad, "méchante", "méch", "m"},
   {give_give, "donnant-donnant", "d-d", "dd"},
   {distrustful, "méfiante", "méf", "m"},
   {give_give_hard, "donnant-donnant-dur", "d-d dur", "ddd"},
   {spiteful, "rancunière", "ranc", "r"},
   {period_nice, "périodique gentille", "p gent", "pg"},
   {period_bad, "périodique méchante", "p méch", "pm"},
   {majority_nice, "majorité mou", "maj mou", "mm"},
   {majority_bad, "majorité dur", "maj dur", "md"},
   {poll, "sondeur", "sond", "s"}
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
        perror("iterate_dilemma");
        result=-1;
        goto end;
    }

    for(int i=0; i<n; i++) {
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
        perror("try_strategies");
        return -1;
    }

    for(int i=0; i<n_strategies; i++) {
        for(int j=0; j<n_strategies; j++) {
            if(iterate_dilemma(strategies[i].strat, strategies[j].strat, n,
                               rewards, &results[i][j], NULL)) {
                results[i][j] = 0;
            }
        }
    }
    return 0;
}
