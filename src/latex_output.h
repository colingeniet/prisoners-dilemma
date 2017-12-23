#ifndef LATEX_OUTPUT_H_INCLUDED
#define LATEX_OUTPUT_H_INCLUDED

#include <stdio.h>
#include "strategies.h"

/** latex implementation */
#define latex_prog "pdflatex"
/** pdf viewer */
#define pdf_viewer "evince"

int print_result(FILE *f, int n_strategies, int **results,
                 struct strategy_entry *strategies);

int print_cumulated_result(FILE *f, int n_strategies, int **results,
                           struct strategy_entry *strategies);

int print_latex(FILE *f, int n_strategies, int **results,
                struct strategy_entry *strategies);

/** Compiles a latex file.
 * @param file the path of the file to compile.
 * @return -1 on error, the compilation return status otherwise. */
int compile_latex(const char *file);

/** Opens a pdf file.
 * @param file the path of the file to open.
 * @return -1 on error, the pdf viewer return status otherwise. */
int open_pdf(const char *file);

#endif /* end of include guard: LATEX_OUTPUT_H_INCLUDED */
