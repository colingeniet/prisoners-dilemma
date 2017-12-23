#ifndef STRATEGIES_H_INCLUDED
#define STRATEGIES_H_INCLUDED

/* actions are booleans, cooperating is 'true', defecting is 'false'*/
typedef char action;
#define COOP 1
#define DEFECTS 0

/** Strategy decision function type.
 * Decides of an action depending on the past opponent actions.
 * @param n_played the number of iterations previously played.
 * @param hist the opponent actions history, as an array of actions.
 * @param n_coop the number of times the opponent cooperated in previous games.
 * @return the next action for this strategy. */
typedef action (*strategy)(int n_played, action *hist, int n_coop);

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
action periodic_nice(int n_played, action *hist, int n_coop);
/** defects, defects, cooperates, ... */
action periodic_bad(int n_played, action *hist, int n_coop);
/** plays the adversary most played action, cooperates by default */
action majority_nice(int n_played, action *hist, int n_coop);
/** plays the adversary most played action, defects by default */
action majority_bad(int n_played, action *hist, int n_coop);
/** plays defects, cooperates, cooperates.
    then if the adversary cooperated on game 2 and 3 always defects
         else same as give_give */
action poll(int n_played, action *hist, int n_coop);


/** Group up a strategy with various names for display purposes */
struct strategy_entry {
    strategy strat;
    const char *name;
    const char *short_name;
    const char *very_short_name;
};

/** number of standard strategies */
#define N_STRATEGIES 11
/** standard strategies */
struct strategy_entry strategies[N_STRATEGIES];

/** standard rewards */
int default_rewards[2][2];

/** Run two strategies for n steps.
 * @param player1 strategy function for first player.
 * @param player2 strategy function for second player.
 * @param n the number of iterations.
 * @param rewards the array of rewards.
 * @param res1
 * @param res2
 * @return 0 on success, -1 on error. */
int iterate_dilemma(strategy player1, strategy player2, int n,
                    int rewards[2][2], int *res1, int *res2);

/** Run all strategies against each other.
 * @param strategies an array of strategies.
 * @param n_strategies the size of strategies.
 * @param n the number of iterations.
 * @param rewards the array of rewards.
 * @param results an already allocated array.
 *  results[i][j] is filled with the score of strategy i versus strategy j
 * @return 0 on success, -1 on error. */
int try_strategies(struct strategy_entry *strategies, int n_strategies, int n,
                   int rewards[2][2], int **results);


#endif /* end of include guard: STRATEGIES_H_INCLUDED */
