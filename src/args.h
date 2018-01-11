#ifndef ARGS_H_INCLUDED
#define ARGS_H_INCLUDED

#include "town.h"

/** Parameters modified from command line options. */
struct argp_data {
    /** The initial town state. */
    struct town_descriptor *town;
    /** The monitoring port. -1 if none. */
    short mon_port;
};

/** Set program parameters according to command line options. */
struct argp_data parse_arguments(int argc, char **argv);

#endif /* end of include guard: ARGS_H_INCLUDED */
