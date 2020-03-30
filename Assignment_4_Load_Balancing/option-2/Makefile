RM = rm -f
CC = gcc
CFLAGS = -std=c99 -lpthread -lssl -lcrypto -O3 -lm

default: all

all: bin loadbalance

bin:
	mkdir bin

loadbalance: src/loadbalance.c
	$(CC) $< $(CFLAGS) -o bin/loadbalance

report: report.pdf

report.pdf: report/report.tex
	cd report && pdflatex report.tex && pdflatex report.tex

clean:
	$(RM) bin/loadbalance
	$(RM) report/*.aux report/*.log

.PHONY: loadbalance report clean
