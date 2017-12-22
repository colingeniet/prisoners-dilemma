#include "strategies.h"
#include <stdlib.h>
#include <stdio.h>

/** always cooperates */
action nice(int n_played, action *hist, int n_coop) {
    return COOP;
}

/** always defects */
action bad(int n_played, action *hist, int n_coop) {
    return DEFECTS;
}

/** cooperates on first game, then plays same as adversary last action */
action give_give(int n_played, action *hist, int n_coop) {
    return n_played < 1 || hist[n_played-1];
}

/** defects on first game, then plays same as adversary last action */
action distrustful(int n_played, action *hist, int n_coop) {
    return n_played >= 1 && hist[n_played-1];
}

/** cooperates unless the adversary defected in one of the last two  games */
action give_give_hard(int n_played, action *hist, int n_coop) {
    return (n_played < 1 || hist[n_played-1])
        && (n_played < 2|| hist[n_played-2]);
}

/** cooperates as long as the adversary never defects */
action spiteful(int n_played, action *hist, int n_coop) {
    return n_coop == n_played;
}

/** cooperates, cooperates, defects, ... */
action period_nice(int n_played, action *hist, int n_coop) {
    return n_played%3 != 2;
}

/** defects, defects, cooperates, ... */
action period_bad(int n_played, action *hist, int n_coop)  {
    return n_played%3 == 2;
}

/** plays the adversary most played action, cooperates by default */
action majority_nice(int n_played, action *hist, int n_coop) {
    return 2*n_coop >= n_played;
}

/** plays the adversary most played action, defects by default */
action majority_bad(int n_played, action *hist, int n_coop) {
    return 2*n_coop > n_played;
}

/** plays defects, cooperates, cooperates,
    then if the adversary cooperated on game 2 and 3 always defects
         else same as give_give */
action poll(int n_played, action *hist, int n_coop) {
    if(n_played < 3) return !(n_played%3);
    else if(hist[2] && hist[3]) return DEFECTS;
    else return hist[n_played-1];
}

struct strategy_entry strategies[N_STRATEGIES] = {
   {nice, "gentille"},
   {bad, "méchant"},
   {give_give, "donnant-donnant"},
   {distrustful, "méfiante"},
   {give_give_hard, "donnant-donnant-dur"},
   {spiteful, "rancunière"},
   {period_nice, "périodique gentille"},
   {period_bad, "périodique gentille"},
   {majority_nice, "majorité mou"},
   {majority_bad, "majorité dur"},
   {poll, "sondeur"}
};


int iterate_dilemma(strategy player1, strategy player2, int n,
                    int rewards[2][2], int *res1, int *res2) {
    int reward1=0, reward2=0;
    int n_coop1=0, n_coop2=0;
    int result;
    action *hist1=NULL, *hist2=NULL;
    hist1 = (action*)malloc(n*sizeof(action));
    hist2 = (action*)malloc(n*sizeof(action));
    if(!hist1 || !hist2) {
        perror("failed to allocate memory\n");
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
