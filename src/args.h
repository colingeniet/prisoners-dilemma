#ifndef ARGS_H_INCLUDED
#define ARGS_H_INCLUDED

#include "town.h"

struct argp_data {
    struct town_descriptor *town;
    short mon_port;
};

struct argp_data parse_arguments(int argc, char **argv);

#endif /* end of include guard: ARGS_H_INCLUDED */
