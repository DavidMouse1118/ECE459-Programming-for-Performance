RM = rm -f
CC = gcc
CFLAGS = -std=c99 -O2 -g -pthread
CURLFLAGS = -lcurl -I/usr/include/x86_64-linux-gnu

all: solver checker report

solver: bin sudoku sudoku_threads sudoku_multi sudoku_workers

checker: bin verifier verifier_multi

bin:
	mkdir -p bin

sudoku:
	@printf "Compiling sudoku.\n"
	$(CC) $(CFLAGS) sudoku.c common.c -o $@
	mv $@ bin

sudoku_threads:
	@printf "Compiling sudoku_threads.\n"
	$(CC) $(CFLAGS) sudoku_threads.c common.c -o $@ 
	mv $@ bin

sudoku_multi:
	@printf "Compiling sudoku_multi.\n"
	$(CC) $(CFLAGS) sudoku_multi.c common.c -o $@ 
	mv $@ bin

sudoku_workers:
	@printf "Compiling sudoku_workers.\n"
	$(CC) $(CFLAGS) sudoku_workers.c common.c -o $@ 
	mv $@ bin

verifier:
	@printf "Compiling verifier.\n"
	$(CC) $(CFLAGS) verifier.c common.c $(CURLFLAGS) -o $@
	mv $@ bin

verifier_multi:
	@printf "Compiling verifier_multi.\n"
	$(CC) $(CFLAGS) verifier_multi.c common.c $(CURLFLAGS) -o $@
	mv $@ bin

report: report.pdf

report.pdf: report/report.tex
	cd report && pdflatex report.tex && pdflatex report.tex

clean:
	$(RM) -r bin
	$(RM) report/*.aux report/*.log

.PHONY: all solver checker report clean
