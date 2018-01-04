#include "args.h"
#include <argp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define OPT_NAMES 256

struct argp_option options[] = {
    {0,0,0,0,
"Strategies used in the simulation can be selected form the list of \
standard strategies using --allow and --disallow. By default, all \
strategies are allowed. If several options conflict for a strategy, \
only the last is taken into account. Very short strategy names shall \
be used with --allow and --disallow. See --names for the list of names.\n"},
    {"allow", 'a', "STRATS", 0,
"Allow strategies in STRATS. STRATS shall be a comma separated list \
of strategies given by their very short names.\n\
If STRATS is the string \"all\", all strategies are allowed.\n"},
    {"disallow", 'd', "STRATS", 0,
"Disallow strategies in STRATS. STRATS shall be a comma separated list \
of strategies given by their very short names.\n\
If STRATS is the string \"all\", all strategies are disallowed.\n"},
    {"names", OPT_NAMES, 0, 0, "Print names for all standard strategies.\n", 2},
    {"pop", 'p', "[STRATS:]POP", 0,
"Initial population for strategies in STRATS. STRATS shall be a comma \
separated list of strategies given by their very short name. When used \
without STRATS, it is applied to all strategies.\n", 3},
    {"rewards", 'r', "P,T,D,C", 0,
"Set the reward values. Rewards are as follow :\n\
       | defect |  coop  |\n\
defect | P    P | T    D |\n\
 coop  | D    T | C    C |\n\
Default values are P=1, T=5, D=0, C=3.\n"},
    {0}
};

/** Parse argument for option 'allow' and 'disallow' */
int allow_strats(char *strats, struct town_descriptor *town, char allow) {
    if(!strcmp(strats, "all")) {
        for(int i=0; i<town->n_strategies; i++) {
            town->allowed[i] = allow;
        }
        return 0;
    }

    char *token = strtok(strats, ",");
    while(token) {
        char found = 0;
        for(int i=0; i<town->n_strategies; i++) {
            if(!strcmp(token, town->strategies[i].very_short_name)) {
                town->allowed[i] = allow;
                found = 1;
            }
        }
        if(!found) {
            fprintf(stderr, "Unknown strategy %s\n", token);
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, ",");
    }
    return 0;
}

/** Parse argument for option 'pop' */
int set_population(char *arg, struct town_descriptor *town) {
    char *delim = strchr(arg, ':');
    if(delim) {
        char *end;
        long pop = strtol(delim+1, &end, 0);
        if(*end) {
            fprintf(stderr, "Unexpected population value : %s\n", delim+1);
            exit(EXIT_FAILURE);
        }

        *delim = '\0';
        char *token = strtok(arg, ",");
        while(token) {
            char found = 0;
            for(int i=0; i<town->n_strategies; i++) {
                if(!strcmp(token, town->strategies[i].very_short_name)) {
                    town->population[i] = pop;
                    found = 1;
                }
            }
            if(!found) {
                fprintf(stderr, "Unknown strategy %s\n", token);
                exit(EXIT_FAILURE);
            }
            token = strtok(NULL, ",");
        }
    } else {
        char *end;
        long pop = strtol(arg, &end, 0);
        if(*end) {
            fprintf(stderr, "Unexpected population value : %s\n", arg);
            exit(EXIT_FAILURE);
        }

        for(int i=0; i<town->n_strategies; i++) {
            town->population[i] = pop;
        }
    }
    return 0;
}

/** Parse argument for option 'rewards' */
int set_rewards(char *arg, struct town_descriptor *town) {
    char *token, *end;
    int reward;

    token = strtok(arg, ",");
    for(int i=0; i<4; i++) {
        if(!token) {
            fprintf(stderr, "not enough arguments to option --reward\n");
            exit(EXIT_FAILURE);
        }
        reward = strtol(token, &end, 0);
        if(*end) {
            fprintf(stderr, "Unexpected value : %s\n", token);
            exit(EXIT_FAILURE);
        }
        (*town->rewards)[i/2][i%2] = reward;
        token = strtok(NULL, ",");
    }

    if(token) {
        fprintf(stderr, "Too many arguments to option --reward : %s\n", token);
    }

    return 0;
}

void print_names(struct town_descriptor *town) {
    printf("strategy names :\n");
    printf("very short\tshort\t\tfull\n");
    for(int i=0; i<town->n_strategies; i++) {
        printf("%s\t\t%s\t\t%s\n",
               town->strategies[i].very_short_name,
               town->strategies[i].short_name,
               town->strategies[i].name);
    }
}

error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct town_descriptor *town = state->input;

    switch(key) {
    case 'a':
        allow_strats(arg, town, 1);
        break;
    case 'd':
        allow_strats(arg, town, 0);
        break;
    case 'p':
        set_population(arg, town);
        break;
    case 'r':
        set_rewards(arg, town);
        break;
    case OPT_NAMES:
        print_names(town);
        exit(EXIT_SUCCESS);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct argp argp = {options, parse_opt};

struct town_descriptor *parse_arguments(int argc, char **argv) {
    // creates new town descriptor with default values
    struct town_descriptor *town = malloc(sizeof(struct town_descriptor));
    if(!town) {
        perror("parse_arguments");
        return NULL;
    }
    town->n_strategies = N_STRATEGIES;
    town->strategies = strategies;
    town->rewards = &default_rewards;
    town->allowed = malloc(town->n_strategies * sizeof(char));
    if(!town->allowed) {
        perror("parse_arguments");
        free(town);
        return NULL;
    }
    town->population = malloc(town->n_strategies * sizeof(long));
    if(!town->allowed) {
        perror("parse_arguments");
        free(town->allowed);
        free(town);
        return NULL;
    }

    for(int i=0; i<town->n_strategies; i++) {
        town->allowed[i] = 1;
        town->population[i] = 0;
    }

    argp_parse(&argp, argc, argv, 0, 0, town);

    return town;
}
