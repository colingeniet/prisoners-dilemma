#ifndef STRATEGIES_H_INCLUDED
#define STRATEGIES_H_INCLUDED

typedef char action;
#define COOP 1
#define DEFECTS 0

typedef action (*strategy)(int, action*, int);

/** always cooperates */
action nice(int n_played, action *hist, int n_coop);
/** always defects */
action bad(int n_played, action *hist, int n_coop);
/** cooperates on first game, then plays same as adversary last action */
action give_give(int n_played, action *hist, int n_coop);
/** defects on first game, then plays same as adversary last action */
action distrustful(int n_played, action *hist, int n_coop);
/** cooperates unless the adversary defected in one of the last two  games */
action give_give_hard(int n_played, action *hist, int n_coop);
/** cooperates as long as the adversary never defects */
action spiteful(int n_played, action *hist, int n_coop);
/** cooperates, cooperates, defects, ... */
action period_nice(int n_played, action *hist, int n_coop);
/** defects, defects, cooperates, ... */
action period_bad(int n_played, action *hist, int n_coop);
/** plays the adversary most played action, cooperates by default */
action majority_nice(int n_played, action *hist, int n_coop);
/** plays the adversary most played action, defects by default */
action majority_bad(int n_played, action *hist, int n_coop);
/** plays defects, cooperates, cooperates,
    then if the adversary cooperated on game 2 and 3 always defects
         else same as give_give */
action poll(int n_played, action *hist, int n_coop);

#define N_STRATEGIES 11
struct strategy_entry {
    strategy strat;
    const char *name;
};

struct strategy_entry strategies[N_STRATEGIES];


int iterate_dilemma(strategy player1, strategy player2, int n,
                    int rewards[2][2], int *res1, int *res2);

#endif /* end of include guard: STRATEGIES_H_INCLUDED */
