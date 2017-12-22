#ifndef LATEX_OUTPUT_H_INCLUDED
#define LATEX_OUTPUT_H_INCLUDED

#include <stdio.h>
#include "strategies.h"


int print_result(FILE *f, int n_strategies, int **results,
                 struct strategy_entry *strategies);

int print_latex(FILE *f, int n_strategies, int **results,
                struct strategy_entry *strategies);

int compile_latex(const char *file);
int open_pdf(const char *reader, const char *file);

#endif /* end of include guard: LATEX_OUTPUT_H_INCLUDED */
