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


/** Write a latex plot of prisoner dilemma populations evolution.
 * @param f the file to write to.
 * @param n_strategies the number of populations.
 * @param data_file the file in which the plot data is written,
 *   as by print_population_data.
 * @param strategies the array of strategies.
 * @return -1 on error, 0 otherwise. */
int print_population_graph(FILE *f, int n_strategies, char *data_file,
                           struct strategy_entry *strategies);

/** Write plot data of prisoner dilemma populations evolution.
 * @param f the file to write to.
 * @param n_strategies the number of populations.
 * @param n the number of steps of simulation.
 * @param results the result array, as produced by `populations`.
 * @param strategies the array of strategies.
 * @return -1 on error, 0 otherwise. */
int print_population_data(FILE *f, int n_strategies, int n, long **results,
                          struct strategy_entry *strategies);

/** Write plot data of prisoner dilemma populations evolution.
 * @param f the file to write to.
 * @param n_strategies the number of populations.
 * @param n the number of steps of simulation.
 * @param results the result array, as produced by `proportions`.
 * @param strategies the array of strategies.
 * @return -1 on error, 0 otherwise. */
int print_population_data_d(FILE *f, int n_strategies, int n, double **results,
                            struct strategy_entry *strategies);

/** Write complete latex file with prisoner dilemma populations evolution plot.
 * @param f the file to write to.
 * @param n_strategies the number of populations.
 * @param data_file the file in which the plot data is written,
 *   as by print_population_data.
 * @param strategies the array of strategies.
 * @return -1 on error, 0 otherwise. */
int print_population_latex(FILE *f, int n_strategies, char *data_file,
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
