print-%: ; @echo $*=$($*)

.PHONY: all report clean

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------

# EXEC_FLAGS := 5 1 10 1 1
EXEC_FLAGS := 80 2 4000 6 6

#-------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------

# ENABLE_GPERFTOOLS := 1
# ENABLE_DEBUG := 1

OPENSSL     := /usr/include/openssl
OPENSSL_LIB := -lssl -lcrypto

OBJ_DIR := ./bin
SOURCES := $(shell find . -type f -name *.cpp)
OBJECTS := $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

CXXFLAGS := -I./src -I$(OPENSSL) -MMD -MP -std=c++11 -O0 -g -Wall -Wno-sign-compare
LDFLAGS  := $(OPENSSL_LIB) -lpthread
CXX      := g++

ifeq ($(ENABLE_GPERFTOOLS), 1)
LDFLAGS += -lprofiler
endif

ifeq ($(ENABLE_DEBUG), 1)
CXXFLAGS += -DDEBUG
endif

CXXFLAGS_slow :=
LDFLAGS_slow  :=

CXXFLAGS_fast := -DUSE_FAST
LDFLAGS_fast  := 

EXECS := slow fast

#------------------------------------------------------------------------------
# Main Program
#------------------------------------------------------------------------------

all: $(EXECS)

#------------------------------------------------------------------------------
define make_target =

EXEC_$(1) := hackathon_$(1)

$(OBJ_DIR)/%.$(1).o: $$(subst $(OBJ_DIR)/,,%.cpp)
	@mkdir -p $$(dir $$@)
	$$(CXX) $$< -c $$(CXXFLAGS) $(CXXFLAGS_$(1)) -o $$@

$$(EXEC_$(1)): $(OBJECTS:.o=.$(1).o)
	@echo Linking $$@
	$$(CXX) $$^ $$(LDFLAGS) $(LDFLAGS_$(1)) -o $$@
	@echo

DEPENDS_$(1) := $(OBJECTS:.o=.$(1).d)
-include $$(DEPENDS_$(1))

$(1): $$(EXEC_$(1))

run_$(1): $(1)
	@echo
	$$(EXEC_PRELOAD) ./$$(EXEC_$(1)) $$(EXEC_FLAGS)

#--------------------------------------
# gperftools
# https://github.com/gperftools/gperftools
#--------------------------------------
ifeq ($(ENABLE_GPERFTOOLS), 1)

PROFILE_LOCATION_$(1) := prof_$(1).out
EXEC_PRELOAD := LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libprofiler.so CPUPROFILE=$$(PROFILE_LOCATION_$(1))

gprof_$(1): run_$(1)
	@echo
	google-pprof --gv $$(EXEC_$(1)) $$(PROFILE_LOCATION_$(1))
endif

#--------------------------------------
# FlameGraph
# https://github.com/brendangregg/FlameGraph
#--------------------------------------

# Where you downloaded/extracted the flamegraph repo
FLAMEGRAPH_DIR := /opt-src/FlameGraph-master

# -F 10000 (sampling rate in Hz)
# -g (record call stack)
PERF_FLAGS := -F 1000 -g --call-graph dwarf

FLAMEGRAPH_RAW_FILE_$(1) := perf_$(1).data

FLAMEGRAPH_FLAGS := --width 6000 --minwidth 10

fg_$(1): $(1)
	@echo
	perf record $$(PERF_FLAGS) -o $$(FLAMEGRAPH_RAW_FILE_$(1)) -- ./$$(EXEC_$(1)) $$(EXEC_FLAGS)
	perf script -i $$(FLAMEGRAPH_RAW_FILE_$(1)) | $$(FLAMEGRAPH_DIR)/stackcollapse-perf.pl | c++filt | $$(FLAMEGRAPH_DIR)/flamegraph.pl $$(FLAMEGRAPH_FLAGS) > perf_$(1).svg

endef
#------------------------------------------------------------------------------

# To debug the dynamically generated rules, replace "eval" with "info"
# $(foreach exec,$(EXECS),$(info $(call make_target,$(exec))))
$(foreach exec,$(EXECS),$(eval $(call make_target,$(exec))))

report: report.pdf

report.pdf: report/report.tex
	cd report && pdflatex report.tex && pdflatex report.tex

#------------------------------------------------------------------------------
# Clean
#------------------------------------------------------------------------------

clean:
	rm -rf $(OBJ_DIR)
	rm -f prof_*
	rm -f perf_*
	rm -f $(foreach exec,$(EXECS),hackathon_$(exec))
	rm -f report/*.aux report/*.log
