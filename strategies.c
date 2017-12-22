#include "strategies.h"

/** always cooperates */
action nice(int n_played, action *hist, int n_coop, int n_defect) {
    return COOP;
}

/** always defects */
action bad(int n_played, action *hist, int n_coop, int n_defect) {
    return DEFECTS;
}

/** cooperates on first game, then plays same as adversary last action */
action give_give(int n_played, action *hist, int n_coop, int n_defect) {
    return n_played < 1 || hist[n_played-1];
}

/** defects on first game, then plays same as adversary last action */
action distrustful(int n_played, action *hist, int n_coop, int n_defect) {
    return n_played >= 1 && hist[n_played-1];
}

/** cooperates unless the adversary defected in one of the last two  games */
action give_give_hard(int n_played, action *hist, int n_coop, int n_defect) {
    return (n_played < 1 || hist[n_played-1])
        && (n_played < 2|| hist[n_played-2]);
}

/** cooperates as long as the adversary never defects */
action spiteful(int n_played, action *hist, int n_coop, int n_defect) {
    return !n_defect;
}

/** cooperates, cooperates, defects, ... */
action period_nice(int n_played, action *hist, int n_coop, int n_defect) {
    return n_played%3 != 2;
}

/** defects, defects, cooperates, ... */
action period_bad(int n_played, action *hist, int n_coop, int n_defect)  {
    return n_played%3 == 2;
}

/** plays the adversary most played action, cooperates by default */
action majority_nice(int n_played, action *hist, int n_coop, int n_defect) {
    return n_coop >= n_defect;
}

/** plays the adversary most played action, defects by default */
action majority_bad(int n_played, action *hist, int n_coop, int n_defect) {
    return n_coop > n_defect;
}

/** plays defects, cooperates, cooperates,
    then if the adversary cooperated on game 2 and 3 always defects
         else same as give_give */
action poll(int n_played, action *hist, int n_coop, int n_defect) {
    if(n_played < 3) return !(n_played%3);
    else if(hist[2] && hist[3]) return DEFECTS;
    else return hist[n_played-1];
}
