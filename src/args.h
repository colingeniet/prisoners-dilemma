#ifndef ARGS_H_INCLUDED
#define ARGS_H_INCLUDED

#include "town.h"

/** Parameters modified from command line options. */
struct argp_data {
    /** The initial town state. */
    struct town_descriptor *town;
    /** The monitoring port. -1 if none. */
    short mon_port;
    /** port for neighbours incomming connections */
    short in_port;
    /** The array of neighbour towns. */
    char **neighbours;
    short *neighbour_ports;
    int n_neighbours;
    int neighbours_alloc;
    double prob_mig;
};

/** Set program parameters according to command line options. */
struct argp_data parse_arguments(int argc, char **argv);

#endif /* end of include guard: ARGS_H_INCLUDED */
