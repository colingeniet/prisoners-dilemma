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
be used with --allow and --disallow. See --names for the list of names."},
    {"allow", 'a', "STRATS", 0,
"Allow strategies in STRATS. STRATS shall be a comma separated list \
of strategies given by their very short names.\n\
If STRATS is the string \"all\", all strategies are allowed."},
    {"disallow", 'd', "STRATS", 0,
"Disallow strategies in STRATS. STRATS shall be a comma separated list \
of strategies given by their very short names.\n\
If STRATS is the string \"all\", all strategies are disallowed."},

    {"names", OPT_NAMES, 0, 0, "Print names for all standard strategies.", 2},

    {"pop", 'p', "[STRATS:]POP", 0,
"Set initial population for strategies in STRATS. STRATS shall be a comma \
separated list of strategies given by their very short name. When used \
without STRATS, it is applied to all strategies.\n\
If several option conflict for a strategy, only the last is taken into account", 3},

    {"rewards", 'r', "P,T,D,C", 0,
"Set the reward values. Rewards are as follow :\n\
       | defect |  coop  |\n\
defect | P    P | T    D |\n\
 coop  | D    T | C    C |\n\
Default values are P=1, T=5, D=0, C=3.", 4},

    {"neighbour", 'n', "HOST:PORT", 0,
"Add a neighbour town.", 5},

    {"mon", 'm', "PORT", 0,
"Accept a monitoring connection on PORT. If a connection is established, \
simulation data will be sent to it, and is not printed. \n\
Closing the connection cause the program to terminate.", 6},
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

short get_port(char *arg) {
    char *end;
    long val = strtol(arg, &end, 0);
    if(*end) {
        fprintf(stderr, "Unexpected value : %s\n", arg);
        exit(EXIT_FAILURE);
    }
    return val;
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

/** add a neighbour to the neighbours array */
void add_neighbour(char *host, struct argp_data *data) {
    // split the host name
    char *delim = strrchr(host, ':');
    if(!delim) {
        fprintf(stderr, "invalid host name : %s\n", host);
        fprintf(stderr, "excepected syntax : HOST:PORT\n");
        exit(EXIT_FAILURE);
    }
    *delim = '\0';
    char *port_str = delim+1;
    char *end;
    short port = strtol(port_str, &end, 10);
    if(*end) {
        fprintf(stderr, "invalid port : %s\n", port_str);
        exit(EXIT_FAILURE);
    }

    // reallocation
    if(data->n_neighbours >= data->neighbours_alloc) {
        int new_size = (data->neighbours_alloc ? 2*data->neighbours_alloc : 1);
        data->neighbours = realloc(data->neighbours, new_size * sizeof(char*));
        data->neighbour_ports = realloc(data->neighbour_ports, new_size * sizeof(short));
        if(!data->neighbours || !data->neighbour_ports) {
            perror("args");
            exit(EXIT_FAILURE);
        }
        data->neighbours_alloc = new_size;
    }

    data->neighbours[data->n_neighbours] = host;
    data->neighbour_ports[data->n_neighbours] = port;
    data->n_neighbours++;
}

error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct argp_data *data = state->input;
    struct town_descriptor *town = data->town;

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
    case 'm':
        data->mon_port = get_port(arg);
        break;
    case 'n':
        add_neighbour(arg, data);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct argp argp = {options, parse_opt};

struct argp_data parse_arguments(int argc, char **argv) {
    struct argp_data data;

    // creates new town descriptor with default values
    struct town_descriptor *town = malloc(sizeof(struct town_descriptor));
    if(!town) goto fail;
    town->n_strategies = N_STRATEGIES;
    town->strategies = strategies;
    town->rewards = &default_rewards;
    town->allowed = NULL;
    town->population = NULL;

    town->allowed = malloc(town->n_strategies * sizeof(char));
    if(!town->allowed) goto fail;
    town->population = malloc(town->n_strategies * sizeof(long));
    if(!town->allowed) goto fail;

    for(int i=0; i<town->n_strategies; i++) {
        town->allowed[i] = 1;
        town->population[i] = 0;
    }

    data.town = town;
    data.mon_port = -1;
    data.neighbours = NULL;
    data.neighbour_ports = NULL;
    data.n_neighbours = 0;
    data.neighbours_alloc = 0;

    argp_parse(&argp, argc, argv, 0, 0, &data);

    return data;

    fail:
    perror("parse_arguments");
    if(town) {
        free(town->allowed);
        free(town->population);
        free(town);
    }
    exit(EXIT_FAILURE);
}
