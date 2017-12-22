#include "latex_output.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    fprintf(f, "\\begin{tabular}{| ");

    for(int i = 0; i < n_strategy + 1; i++)
        fprintf(f, " c |");
    fprintf(f, "}\n");

    for(int i = 0; i < n_strategy; i++)
        fprintf(f, "& %s ", strat[i].name);
    fprintf(f, "\\\\ \\hline \n");

    for(int i = 0; i < n_strategy; i++) {
        fprintf(f, "%s ", strat[i].name);
        for(int j = 0; j < n_strategy; j++) {
            fprintf(f, "& %d", points[i][j]);
        }
        fprintf(f, "\\\\ \\hline \n");
    }
    fprintf(f, "\\end{tabular}");
    return 0;
}

int print_cumulated_result(FILE *f, int n_strategy, int *points, struct strategy_entry *strat) {
    if(!f || !points || !strat)
        return -1;
    fprintf(f, "\\begin{tabular}{| ");

    for(int i = 0; i < n_strategy; i++)
        fprintf(f, " c |");
    fprintf(f, "}\n");

    for(int i = 0; i < n_strategy; i++)
        fprintf(f, "%s &", strat[i].name);
    fprintf(f, " Total ");
    fprintf(f, "\\\\ \\hline \n");

    for(int i = 0; i < n_strategy; i++) {
        fprintf(f, "%d &", points[i]);
    }
    fprintf(f, "%d", sum(points, n_strategy));
    fprintf(f, "\\\\ \\hline");
    fprintf(f, "\\end{tabular}");
    return 0;
}

int compile_latex(const char *path)
{
    if(!path)
        return -1;
    char *cmd = malloc(strlen(path) + 10);
    if(!cmd)
        return -1;
    strcpy(cmd, "pdflatex ");
    strcat(cmd, path);
    return system(cmd);
}

int open_pdf(const char *cmd_name, const char *path)
{
    if(!path || !cmd_name)
        return -1;
    int len = strlen(cmd_name);
    char *cmd = malloc(strlen(path) + len + 2);
    if(!cmd)
        return -1;
    strcpy(cmd, cmd_name);
    cmd[len + 1] = ' ';
    cmd[len] = 0;
    strcat(cmd, path);
    return system(cmd);
}
