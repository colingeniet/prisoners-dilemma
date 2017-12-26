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

int print_result(FILE *f, int n_strategy, int **points, struct strategy_entry *strat) {
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

int print_cumulated_result(FILE *f, int n_strategy, int **points, struct strategy_entry *strat) {
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


void print_latex_preamble(FILE *f) {
    fprintf(f, "\\documentclass[10pt]{article}\n");
    fprintf(f, "\\usepackage[utf8]{inputenc}\n");
    fprintf(f, "\\usepackage[T1]{fontenc}\n");
    fprintf(f, "\\usepackage[french]{babel}\n");
    fprintf(f, "\\usepackage[table]{xcolor}\n\n");
    fprintf(f, "\\begin{document}\n");
    fprintf(f, "\\begin{center}\n");
}

void print_latex_end(FILE *f) {
    fprintf(f, "\\end{center}\n");
    fprintf(f, "\\end{document}\n");
}

int print_latex(FILE *f, int n_strategies, int **results,
                struct strategy_entry *strategies) {
    print_latex_preamble(f);
    print_cumulated_result(f, n_strategies, results, strategies);
    print_latex_end(f);
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
