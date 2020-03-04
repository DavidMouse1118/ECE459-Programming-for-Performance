RM = rm -f

all: part1 part2 report

part1:
	@printf "Compiling Part 1 jwtcracker OpenCL\n"
	make -C q1 jwtcracker_ocl

part2:
	@printf "Compiling Part 2 protons OpenCL\n"
	make -C q2 protons_ocl

report: report.pdf

report.pdf: report/report.tex
	cd report && pdflatex report.tex && pdflatex report.tex
	mv report/report.pdf report.pdf

clean:
	$(RM) -r bin
	$(RM) report/*.aux report/*.log
	make -C q1 clean
	make -C q2 clean

.PHONY: all part1 part2 report clean
