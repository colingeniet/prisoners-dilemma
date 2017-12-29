#include "population.h"
#include "strategies.h"
#include "latex_output.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


#define latex_output "populations.tex"
#define pdf_output "populations.pdf"

int main(int argc, char **argv) {
    if(argc < 2 || argc > 3) {
        fprintf(stderr, "usage : populations <steps> [<initial population>]\n");
        return 1;
    }
    int steps = atoi(argv[1]);

    if(argc == 3) {
        /* simulate discrete populations */
        long init_pop = atol(argv[2]);
        long *initial_pop;
        long **result;
        int ret = 0;
        FILE *output;

        initial_pop = malloc(N_STRATEGIES * sizeof(long));
        if(!initial_pop) {
            perror("populations_simulation");
            ret = 1;
            goto end_pop;
        }
        for(int i=0; i<N_STRATEGIES; i++) {
            initial_pop[i] = init_pop;
        }

        result = multi_mallocv(2, (size_t)steps, (size_t)N_STRATEGIES*sizeof(long));
        if(!result) {
            perror("populations_simulation");
            ret = 1;
            goto end_pop;
        }

        populations(strategies, N_STRATEGIES, steps, default_rewards, initial_pop, result);

        output = fopen(latex_output, "w");
        if(!output) {
            perror("populations_simulation");
            ret = 1;
            goto end_pop;
        }
        print_population_latex(output, N_STRATEGIES, steps, result, strategies);
        fclose(output);

        if(!(ret = compile_latex(latex_output)))
            ret = open_pdf(pdf_output);

        end_pop:
        free(initial_pop);
        multi_freev(result, 2, (size_t)steps);
        return ret;
    } else {
        /* simulate proportions */
        double *initial_prop;
        double **result;
        int ret = 0;
        FILE *output;

        initial_prop = malloc(N_STRATEGIES * sizeof(double));
        if(!initial_prop) {
            perror("populations_simulation");
            ret = 1;
            goto end_prop;
        }
        for(int i=0; i<N_STRATEGIES; i++) {
            initial_prop[i] = (double)1/N_STRATEGIES;
        }

        result = multi_mallocv(2, (size_t)steps, (size_t)N_STRATEGIES*sizeof(double));
        if(!result) {
            perror("populations_simulation");
            ret = 1;
            goto end_prop;
        }

        proportions(strategies, N_STRATEGIES, steps, default_rewards, initial_prop, result);

        output = fopen(latex_output, "w");
        if(!output) {
            perror("populations_simulation");
            ret = 1;
            goto end_prop;
        }
        print_population_latex_d(output, N_STRATEGIES, steps, result, strategies);
        fclose(output);

        if(!(ret = compile_latex(latex_output)))
            ret = open_pdf(pdf_output);

        end_prop:
        free(initial_prop);
        multi_freev(result, 2, (size_t)steps);
        return ret;
    }
}
