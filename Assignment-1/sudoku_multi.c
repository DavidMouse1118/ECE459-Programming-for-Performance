/*
 * A simple backtracking sudoku solver.  Accepts input with cells, dot (.)
 * to represent blank spaces and rows separated by newlines. Output format is
 * the same, only solved, so there will be no dots in it.
 *
 * Copyright (c) Mitchell Johnson (ehntoo@gmail.com), 2012
 * Modifications 2019 by Jeff Zarnett (jzarnett@uwaterloo.ca) for the purposes
 * of the ECE 459 assignment.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include "common.h"

/* Check the common header for the definition of puzzle */

/* Check if current number is valid in this position;
 * returns 1 if yes, 0 if not */
int is_valid(int number, puzzle *p, int row, int column);

int solve(puzzle *p, int row, int column);

void write_to_file(puzzle *p, FILE *outputfile);

int main(int argc, char **argv) {
    FILE *inputfile;
    FILE *outputfile;
    puzzle *p;
    int current_puzzle = 0;

    /* Parse arguments */
    int c;
    int num_threads = 1;
    char *filename = NULL;
    while ((c = getopt(argc, argv, "t:i:")) != -1) {
        switch (c) {
            case 't':
                num_threads = strtoul(optarg, NULL, 10);
                if (num_threads == 0) {
                    printf("%s: option requires an argument > 0 -- 't'\n", argv[0]);
                    return EXIT_FAILURE;
                }
                break;
            case 'i':
                filename = optarg;
                break;
            default:
                return -1;
        }
    }

    /* Open Files */
    inputfile = fopen(filename, "r");
    if (inputfile == NULL) {
        printf("Unable to open input file.\n");
        return EXIT_FAILURE;
    }
    outputfile = fopen("output.txt", "w+");
    if (outputfile == NULL) {
        printf("Unable to open output file.\n");
        return EXIT_FAILURE;
    }

    /* Main loop - solve puzzle, write to file.
     * The read_next_puzzle function is defined in the common header */
    while ((p = read_next_puzzle(inputfile)) != NULL) {
        current_puzzle++;
        if (solve(p, 0, 0)) {
            write_to_file(p, outputfile);
        } else {
            printf("Illegal sudoku (number %d in the file) (or a broken algorithm)\n", current_puzzle);
        }
        free(p);
    }

    fclose( inputfile );
    fclose( outputfile );
    return 0;
}

/*
 * A recursive function that does all the gruntwork in solving
 * the puzzle.
 */
int solve(puzzle *p, int row, int column) {
    int nextNumber = 1;
    /*
     * Have we advanced past the puzzle?  If so, hooray, all
     * previous cells have valid contents!  We're done!
     */
    if (9 == row) {
        return 1;
    }

    /*
     * Is this element already set?  If so, we don't want to
     * change it.
     */
    if (p->content[row][column]) {
        if (column == 8) {
            if (solve(p, row + 1, 0)) return 1;
        } else {
            if (solve(p, row, column + 1)) return 1;
        }
        return 0;
    }

    /*
     * Iterate through the possible numbers for this empty cell
     * and recurse for every valid one, to test if it's part
     * of the valid solution.
     */
    for (; nextNumber < 10; nextNumber++) {
        if (is_valid(nextNumber, p, row, column)) {
            p->content[row][column] = nextNumber;
            if (column == 8) {
                if (solve(p, row + 1, 0)) return 1;
            } else {
                if (solve(p, row, column + 1)) return 1;
            }
            p->content[row][column] = 0;
        }
    }
    return 0;
}

/*
 * Checks to see if a particular value is presently valid in a
 * given position.
 */
int is_valid(int number, puzzle *p, int row, int column) {
    int modRow = 3 * (row / 3);
    int modCol = 3 * (column / 3);
    int row1 = (row + 2) % 3;
    int row2 = (row + 4) % 3;
    int col1 = (column + 2) % 3;
    int col2 = (column + 4) % 3;

    /* Check for the value in the given row and column */
    for (int i = 0; i < 9; i++) {
        if (p->content[i][column] == number) return 0;
        if (p->content[row][i] == number) return 0;
    }

    /* Check the remaining four spaces in this sector */
    if (p->content[row1 + modRow][col1 + modCol] == number) return 0;
    if (p->content[row2 + modRow][col1 + modCol] == number) return 0;
    if (p->content[row1 + modRow][col2 + modCol] == number) return 0;
    if (p->content[row2 + modRow][col2 + modCol] == number) return 0;
    return 1;
}

/*
 * Convenience function to print out the puzzle.
 */
void write_to_file(puzzle *p, FILE *outputfile) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (8 == j) {
                fprintf(outputfile, "%d\n", p->content[i][j]);
            } else {
                fprintf(outputfile, "%d", p->content[i][j]);
            }
        }
    }
    fprintf(outputfile, "\n\n");
}

