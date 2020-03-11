#include <stdio.h>
#include <stdlib.h>
#include "common.h"

puzzle *read_next_puzzle(FILE *inputfile) {
    puzzle *p = malloc(sizeof(puzzle));
    char temp[10];
    for (int i = 0; i < 9; i++) {
        int read = fscanf(inputfile, "%9c\n", temp);
        if (read != 1) {
            /* Reached EOF */
            free(p);
            return NULL;
        }
        for (int j = 0; j < 9; j++) {
            p->content[i][j] = temp[j] == '.'
                               ? 0
                               : temp[j] - '0';
        }
    }
    return p;
}