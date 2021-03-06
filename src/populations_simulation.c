#include "population.h"
#include "strategies.h"
#include "latex_output.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


#define data_output "populations.dat"
#define latex_output "populations.tex"
#define pdf_output "populations.pdf"

int main(int argc, char **argv) {
    if(argc < 2 || argc > 3) {
        fprintf(stderr, "usage : populations <steps> [<initial population>]\n");
        return 1;
    }
    /* steps is actually the number of generations in the simulation,
     * including the initial one. To get the excepted result,
     * we need one more. */
    int steps = atoi(argv[1]) + 1;

    if(argc == 3) {
        /* simulate discrete populations */
        long init_pop = atol(argv[2]);
        long *initial_pop;
        long **result;
        int ret = 0;
        FILE *output;

        initial_pop = malloc(N_STRATEGIES * sizeof(long));
        if(!initial_pop) fatal_perror("malloc");
        for(int i=0; i<N_STRATEGIES; i++) {
            initial_pop[i] = init_pop;
        }

        result = multi_malloc(2, (size_t)steps, (size_t)N_STRATEGIES*sizeof(long));
        if(!result) fatal_perror("malloc");

        populations(strategies, N_STRATEGIES, steps, default_rewards, initial_pop, result);

        output = fopen(data_output, "w");
        if(!output) fatal_perror("fopen");
        print_population_data(output, N_STRATEGIES, steps, result, strategies);
        fclose(output);

        output = fopen(latex_output, "w");
        if(!output) fatal_perror("fopen");
        print_population_latex(output, N_STRATEGIES, data_output, strategies);
        fclose(output);

        if(!(ret = compile_latex(latex_output)))
            ret = open_pdf(pdf_output);

        free(initial_pop);
        multi_free(result, 2, (size_t)steps);
        return ret;
    } else {
        /* simulate proportions */
        double *initial_prop;
        double **result;
        int ret = 0;
        FILE *output;

        initial_prop = malloc(N_STRATEGIES * sizeof(double));
        if(!initial_prop) fatal_perror("malloc");
        for(int i=0; i<N_STRATEGIES; i++) {
            initial_prop[i] = (double)1/N_STRATEGIES;
        }

        result = multi_malloc(2, (size_t)steps, (size_t)N_STRATEGIES*sizeof(double));
        if(!result) fatal_perror("malloc");

        proportions(strategies, N_STRATEGIES, steps, default_rewards, initial_prop, result);

        output = fopen(data_output, "w");
        if(!output) fatal_perror("fopen");
        print_population_data_d(output, N_STRATEGIES, steps, result, strategies);
        fclose(output);

        output = fopen(latex_output, "w");
        if(!output) fatal_perror("fopen");
        print_population_latex(output, N_STRATEGIES, data_output, strategies);
        fclose(output);

        if(!(ret = compile_latex(latex_output)))
            ret = open_pdf(pdf_output);

        free(initial_prop);
        multi_free(result, 2, (size_t)steps);
        return ret;
    }
}
