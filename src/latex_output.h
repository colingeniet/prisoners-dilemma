#ifndef LATEX_OUTPUT_H_INCLUDED
#define LATEX_OUTPUT_H_INCLUDED

#include <stdio.h>
#include "strategies.h"

/** latex implementation */
#define latex_prog "pdflatex"
/** pdf viewer */
#define pdf_viewer "evince"

/** Write a latex comparison table of prisoner dilemma strategies.
 * @param f the file to write to.
 * @param n_strategies the number of strategies compared.
 * @param results the array of results, as filled by try_strategies.
 * @param strategies the array of strategies.
 * @return -1 on error, 0 otherwise. */
int print_strategies_table(FILE *f, int n_strategies, int **results,
                           struct strategy_entry *strategies);

/** Write a latex comparison table of prisoner dilemma strategies with totals.
 * @param f the file to write to.
 * @param n_strategies the number of strategies compared.
 * @param results the array of results, as filled by try_strategies.
 * @param strategies the array of strategies.
 * @return -1 on error, 0 otherwise. */
int print_strategies_table_total(FILE *f, int n_strategies, int **results,
                                 struct strategy_entry *strategies);

/** Write a complete latex file with the strategies comparison table.
 * @param f the file to write to.
 * @param n_strategies the number of strategies compared.
 * @param results the array of results, as filled by try_strategies.
 * @param strategies the array of strategies.
 * @return -1 on error, 0 otherwise. */
int print_strategies_latex(FILE *f, int n_strategies, int **results,
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
