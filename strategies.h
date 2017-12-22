#ifndef STRATEGIES_H_INCLUDED
#define STRATEGIES_H_INCLUDED

typedef char action;
#define COOP 1
#define DEFECTS 0

typedef action (*strategy)(int, action*, int, int);

/** always cooperates */
action nice(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** always defects */
action bad(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** cooperates on first game, then plays same as adversary last action */
action give_give(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** defects on first game, then plays same as adversary last action */
action distrustful(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** cooperates unless the adversary defected in one of the last two  games */
action give_give_hard(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** cooperates as long as the adversary never defects */
action spiteful(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** cooperates, cooperates, defects, ... */
action period_nice(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** defects, defects, cooperates, ... */
action period_bad(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** plays the adversary most played action, cooperates by default */
action majority_nice(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** plays the adversary most played action, defects by default */
action majority_bad(int n_played, action *adv_hist, int adv_coop, int adv_defect);
/** plays defects, cooperates, cooperates,
    then if the adversary cooperated on game 2 and 3 always defects
         else same as give_give */
action poll(int n_played, action *adv_hist, int adv_coop, int adv_defect);

#endif /* end of include guard: STRATEGIES_H_INCLUDED */
