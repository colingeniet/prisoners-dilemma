#include "latex_output.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MAX_TABLE_COLUMNS 6

int sum(int *ary, int n)
{
    int s = 0;
    for(int i = 0; i < n; i++)
        s += ary[i];
    return s;
}

int print_strategies_table(FILE *f, int n_strategy, int **points, struct strategy_entry *strat) {
    if(!f || !points || !strat)
        return -1;

    for(int table=0; table < n_strategy; table += MAX_TABLE_COLUMNS) {
        int limit = n_strategy<table+MAX_TABLE_COLUMNS ?
                        n_strategy : table+MAX_TABLE_COLUMNS;

        fprintf(f, "\\rowcolors{2}{white}{gray}\n");
        fprintf(f, "\\begin{tabular}{|c|");

        for(int i = table; i < limit; i++)
            fprintf(f, "c");
        fprintf(f, "|}\n\\hline\n");

        for(int i = table; i < limit; i++)
            fprintf(f, "& %s ", strat[i].short_name);
        fprintf(f, "\\\\ \\hline \n");

        for(int i = 0; i < n_strategy; i++) {
            fprintf(f, "%s ", strat[i].name);
            for(int j = table; j<limit; j++) {
                fprintf(f, "& %d", points[i][j]);
            }
            fprintf(f, "\\\\\n");
        }
        fprintf(f, "\\hline\n");
        fprintf(f, "\\end{tabular}\n");
        fprintf(f, "\\\\\n");
    }
    return 0;
}

int print_strategies_table_total(FILE *f, int n_strategy, int **points, struct strategy_entry *strat) {
    if(!f || !points || !strat)
        return -1;


    for(int table=0; table <= n_strategy; table += MAX_TABLE_COLUMNS) {
        int limit = n_strategy < table+MAX_TABLE_COLUMNS ?
                        n_strategy+1 : table+MAX_TABLE_COLUMNS;

        fprintf(f, "\\rowcolors{2}{white}{lightgray}\n");
        fprintf(f, "\\begin{tabular}{|c|");

        for(int i = table; i < limit; i++)
            fprintf(f, "c");
        fprintf(f, "|}\n\\hline\n");

        for(int i = table; i < limit; i++) {
            if(i == n_strategy)
                fprintf(f, "& total ");
            else
                fprintf(f, "& %s ", strat[i].short_name);
        }
        fprintf(f, "\\\\ \\hline \n");

        for(int i = 0; i < n_strategy; i++) {
            fprintf(f, "%s ", strat[i].name);
            for(int j = table; j < limit; j++) {
                if(j == n_strategy)
                    fprintf(f, "& %d", sum(points[i], n_strategy));
                else
                    fprintf(f, "& %d", points[i][j]);
            }
            fprintf(f, "\\\\\n");
        }
        fprintf(f, "\\hline\n");
        fprintf(f, "\\end{tabular}\n");
        fprintf(f, "\\\\\n");
    }
    return 0;
}


void print_latex_table_preamble(FILE *f) {
    fprintf(f, "\\documentclass[10pt]{article}\n");
    fprintf(f, "\\usepackage[utf8]{inputenc}\n");
    fprintf(f, "\\usepackage[T1]{fontenc}\n");
    fprintf(f, "\\usepackage[french]{babel}\n");
    fprintf(f, "\\usepackage[table]{xcolor}\n\n");
    fprintf(f, "\\begin{document}\n");
    fprintf(f, "\\begin{center}\n");
}

void print_latex_table_end(FILE *f) {
    fprintf(f, "\\end{center}\n");
    fprintf(f, "\\end{document}\n");
}

int print_strategies_latex(FILE *f, int n_strategies, int **results,
                           struct strategy_entry *strategies) {
    print_latex_table_preamble(f);
    print_strategies_table_total(f, n_strategies, results, strategies);
    print_latex_table_end(f);
    return 0;
}




int print_population_graph(FILE *f, int n_strategies, char *data_file,
                           struct strategy_entry *strategies) {
    fprintf(f, "\\begin{axis}[\n");
    fprintf(f, "    title={Évolution des populations},\n");
    fprintf(f, "    ymin=0,xmin=0,\n");
    fprintf(f, "    cycle list name=color list,\n");
    fprintf(f, "    legend style={\n");
    fprintf(f, "        legend pos=outer north east,\n");
    fprintf(f, "    },\n");
    fprintf(f, "    legend entries={\n");
    for(int strat=0; strat<n_strategies; strat++) {
        fprintf(f, "        %s,\n", strategies[strat].short_name);
    }
    fprintf(f, "    }]\n");

    for(int strat=0; strat<n_strategies; strat++) {
        fprintf(f, "    \\addplot table [x=gen,y=%s] {%s};\n",
                strategies[strat].very_short_name, data_file);
    }
    fprintf(f, "\\end{axis}\n");

    return 0;
}


int print_population_data(FILE *f, int n_strategies, int n, long **results,
                          struct strategy_entry *strategies) {
    fprintf(f, "gen");
    for(int strat=0; strat<n_strategies; strat++) {
        fprintf(f, "\t%s", strategies[strat].very_short_name);
    }
    fprintf(f, "\n");
    for(int step=0; step<n; step++) {
        fprintf(f, "%d", step);
        for(int strat=0; strat<n_strategies; strat++) {
            fprintf(f, "\t%ld", results[step][strat]);
        }
        fprintf(f, "\n");
    }
    return 0;
}

int print_population_data_d(FILE *f, int n_strategies, int n, double **results,
                            struct strategy_entry *strategies) {
    fprintf(f, "gen");
    for(int strat=0; strat<n_strategies; strat++) {
        fprintf(f, "\t%s", strategies[strat].very_short_name);
    }
    fprintf(f, "\n");
    for(int step=0; step<n; step++) {
        fprintf(f, "%d", step);
        for(int strat=0; strat<n_strategies; strat++) {
            fprintf(f, "\t%f", results[step][strat]);
        }
        fprintf(f, "\n");
    }
    return 0;
}

void print_latex_graph_preamble(FILE *f) {
    fprintf(f, "\\documentclass[10pt]{article}\n");
    fprintf(f, "\\usepackage[utf8]{inputenc}\n");
    fprintf(f, "\\usepackage[T1]{fontenc}\n");
    fprintf(f, "\\usepackage[french]{babel}\n");
    fprintf(f, "\\usepackage{pgfplots}\n\n");
    fprintf(f, "\\pgfplotsset{width=10cm,compat=1.5}\n");
    fprintf(f, "\\begin{document}\n");
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\begin{tikzpicture}\n");
}

void print_latex_graph_end(FILE *f) {
    fprintf(f, "\\end{tikzpicture}\n");
    fprintf(f, "\\end{center}\n");
    fprintf(f, "\\end{document}\n");
}

int print_population_latex(FILE *f, int n_strategies, char *data_file,
                           struct strategy_entry *strategies)
{
    print_latex_graph_preamble(f);
    print_population_graph(f, n_strategies, data_file, strategies);
    print_latex_graph_end(f);
    return 0;
}


int compile_latex(const char *path)
{
    if(!path)
        return -1;
    int len = strlen(latex_prog);
    char *cmd = malloc(strlen(path) + len + 2);
    if(!cmd)
        return -1;
    strcpy(cmd, latex_prog);
    cmd[len] = ' ';
    cmd[len + 1] = 0;
    strcat(cmd, path);
    int ret = system(cmd);
    free(cmd);
    if(ret == -1) return -1;
    else return WEXITSTATUS(ret);
}

int open_pdf(const char *path)
{
    if(!path)
        return -1;
    int len = strlen(pdf_viewer);
    char *cmd = malloc(strlen(path) + len + 2);
    if(!cmd)
        return -1;
    strcpy(cmd, pdf_viewer);
    cmd[len] = ' ';
    cmd[len + 1] = 0;
    strcat(cmd, path);
    int ret = system(cmd);
    free(cmd);
    if(ret == -1) return -1;
    else return WEXITSTATUS(ret);
}
