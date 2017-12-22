#ifndef STRATEGIES_H_INCLUDED
#define STRATEGIES_H_INCLUDED

typedef char action;
#define COOP 1
#define DEFECTS 0

typedef action (*strategy)(int, action*, int, int);

action nice(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action bad(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action give_give(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action give_give_hard(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action retaliate(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action period_nice(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action period_bad(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action majority_nice(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action majority_bad(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action distrusfull(int n_played, action *adv_hist, int adv_coop, int adv_defect);
action poll(int n_played, action *adv_hist, int adv_coop, int adv_defect);

#endif /* end of include guard: STRATEGIES_H_INCLUDED */
