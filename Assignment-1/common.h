#ifndef SUDOKU_COMMON_H
#define SUDOKU_COMMON_H
typedef struct {
    int content[9][9];
} puzzle;

puzzle *read_next_puzzle(FILE *inputfile);

#endif //SUDOKU_COMMON_H
