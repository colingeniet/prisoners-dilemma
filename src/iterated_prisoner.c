#include <stdio.h>
#include <stdlib.h>
#include "strategies.h"
#include "latex_output.h"
#include "utils.h"

#define latex_output "iterated_dilemma.tex"
#define pdf_output "iterated_dilemma.pdf"

int main(int argc, char **argv) {
    int ret, n;
    int **results = NULL;
    FILE *output = NULL;

    if(argc != 2) {
        fprintf(stderr, "usage : iterated_prisoner <steps>\n");
        ret = 1;
        goto end;
    }
    n = atoi(argv[1]);

    results = (int**)multi_malloc(2, (size_t)N_STRATEGIES,
                                   (size_t)N_STRATEGIES*sizeof(int));
    if(!results) {
        perror("iterated_prisoner");
        ret = 1;
        goto end;
    }

    try_strategies(strategies, N_STRATEGIES, n, default_rewards, results);

    output = fopen(latex_output, "w");
    if(!output) {
        perror("iterated_prisoner");
        ret = 1;
        goto end;
    }
    print_strategies_latex(output, N_STRATEGIES, results, strategies);
    fclose(output);

    if(!(ret = compile_latex(latex_output)))
        ret = open_pdf(pdf_output);

    end:
    multi_free(results, 2, (size_t)N_STRATEGIES);
    return ret;
}
