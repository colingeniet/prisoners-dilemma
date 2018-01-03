#include "args.h"
#include <argp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct argp_option options[] = {
    {"allow", 'a', "STRATS", 0,
"Allow strategies in STRATS. STRATS shall be a comma separated list \
of strategies given by their very short names."},
    {"pop", 'p', "[STRATS:]POP", 0,
"Initial population for strategies in STRATS. STRATS shall be a comma \
separated list of strategies given by their short name. When used \
without STRATS, it is applied to all strategies"},
    {0}
};

/** Parse a comma separated list of strategy names, and allow them in town.
 *  Very short strategy names are used. */
int allow_strats(char *strats, struct town_descriptor *town) {
    char *token = strtok(strats, ",");
    while(token) {
        char found = 0;
        for(int i=0; i<town->n_strategies; i++) {
            if(!strcmp(token, town->strategies[i].very_short_name)) {
                town->allowed[i] = 1;
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

int set_population(char *arg, struct town_descriptor *town) {
    char *delim = strchr(arg, ':');
    if(delim) {
        char *end;
        long pop = strtol(delim+1, &end, 0);
        if(*end) {
            fprintf(stderr, "Unexcpected population value : %s", delim+1);
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
            fprintf(stderr, "Unexcpected population value : %s", arg);
            exit(EXIT_FAILURE);
        }

        for(int i=0; i<town->n_strategies; i++) {
            town->population[i] = pop;
        }
    }
    return 0;
}

error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct town_descriptor *town = state->input;

    switch(key) {
    case 'a':
        allow_strats(arg, town);
        break;
    case 'p':
        set_population(arg, town);
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
        town->allowed[i] = 0;
        town->population[i] = 0;
    }

    argp_parse(&argp, argc, argv, 0, 0, town);

    return town;
}
