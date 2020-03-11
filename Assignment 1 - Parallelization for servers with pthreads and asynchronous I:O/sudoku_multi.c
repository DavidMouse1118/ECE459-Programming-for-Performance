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


struct thread_args {
    int row;
    int col;
    puzzle *p;
};

int row_1 = 0;
int col_1 = 0;
int row_2 = 0;
int col_2 = 1;

puzzle *solution_p;

int MAX_JOBS = 81;
int num_threads = 1;
int active_threads = 0;
int current_job = 0;

pthread_mutex_t active_threads_lock;
pthread_cond_t cond;

/* Check the common header for the definition of puzzle */

/* Check if current number is valid in this position;
 * returns 1 if yes, 0 if not */
int is_valid(int number, puzzle *p, int row, int column);

int solve(puzzle *p, int row, int column);

void write_to_file(puzzle *p, FILE *outputfile);

void *solve_thread(void *argp);

void find_first_two_empty_space(puzzle *p);

void solve_multi_thread(puzzle *p);

int main(int argc, char **argv) {
    FILE *inputfile;
    FILE *outputfile;
    puzzle *p;
    int current_puzzle = 0;

    /* Parse arguments */
    int c;
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

        solve_multi_thread(p);

        write_to_file(solution_p, outputfile);

        free(p);
        free(solution_p);
    }

    fclose( inputfile );
    fclose( outputfile );
    return 0;
}

void solve_multi_thread(puzzle *p) {
    pthread_t tid[MAX_JOBS];

    // Find the first two empty space
    find_first_two_empty_space(p);


    for (int nextNumber_1 = 1; nextNumber_1 < 10; nextNumber_1++) {
        for (int nextNumber_2 = 1; nextNumber_2 < 10; nextNumber_2++) {
            if (is_valid(nextNumber_1, p, row_1, col_1) == 0) {
                continue;
            }
            
            p->content[row_1][col_1] = nextNumber_1;

            if (is_valid(nextNumber_2, p, row_2, col_2) == 0) {
                p->content[row_1][col_1] = 0;
                continue;
            }

            p->content[row_2][col_2] = nextNumber_2;

            puzzle *copied_p = malloc(sizeof(puzzle));
            memcpy(copied_p, p, sizeof(puzzle));

            struct thread_args *args = malloc(sizeof *args);

            /* Block on condition variable until there are insufficient workers running */
            pthread_mutex_lock(&active_threads_lock);
            while (active_threads >= num_threads) {
                pthread_cond_wait(&cond, &active_threads_lock);
            }

            active_threads ++;
            pthread_mutex_unlock(&active_threads_lock);

            args->row = row_2;
            args->col = col_2;
            args->p = copied_p;

            pthread_create(&tid[current_job], NULL, solve_thread, args);
            current_job ++;
            
            p->content[row_1][col_1] = 0;
            p->content[row_2][col_2] = 0;
        }
    }

    for (int i = 0; i < current_job; i++) {
        pthread_join(tid[i], NULL);
    }

    active_threads = 0;
    current_job = 0;
}

void *solve_thread(void *argp) {
    struct thread_args *args = argp;

    int row = args->row;
    int col = args->col;
    puzzle *p = args->p;

    free(args);

    if (col == 8) {
        if (solve(p, row + 1, 0)) {
            solution_p = p;
        } else {
            free(p);
        }
    } else {
        if (solve(p, row, col + 1)) {
            solution_p = p;
        } else {
            free(p);
        }
    }

    pthread_mutex_lock(&active_threads_lock);
    /* Worker is about to exit, so decrement count and wakeup main thread */
    active_threads --;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&active_threads_lock);
}

void find_first_two_empty_space(puzzle *p) {
    int empty_count = 0;
    for (int row = 0; row < 9; row ++) {
        for (int col = 0; col < 9; col ++) {
            if (p->content[row][col]) {
                continue;
            }

            if (empty_count == 0) {
                row_1 = row;
                col_1 = col;
                empty_count ++;
            } else if (empty_count == 1) {
                row_2 = row;
                col_2 = col;
                empty_count ++;
            } else {
                return;
            }
        }
    }
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

