#include "random.h"
#include <stdio.h>
#include <stdlib.h>

double rand_fun(void) {
    return (double) rand() / RAND_MAX;     
}

int randint(int a, int b) {
    return (int) rand_fun() * (b - a);
}

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = *tmp;
}

void shuffle(int ary[], int size) {
    for(int i = 0; i < size; i++)
        swap(&ary[i], &ary[randint(0, i)]);
}
