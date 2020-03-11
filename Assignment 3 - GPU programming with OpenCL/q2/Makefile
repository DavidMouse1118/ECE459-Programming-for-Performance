print-%: ; @echo $*=$($*)

#-------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------

OBJ_DIR :=./bin
SOURCES := $(shell find . -type f -name *.cpp)
OBJECTS := $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
DEPENDS := $(OBJECTS:.o=.d)

CXXFLAGS := -I./src -MMD -MP -std=c++0x -g -Wall -Wno-sign-compare
LDFLAGS  :=
CXX      := g++

CXXFLAGS_sin := -Wno-unknown-pragmas
LDFLAGS_sin  :=

CXXFLAGS_omp := -DUSE_OPENMP -fopenmp
LDFLAGS_omp  := -fopenmp

CXXFLAGS_ocl := -DUSE_OPENCL
LDFLAGS_ocl  := -lOpenCL

EXECS := sin omp ocl

# You will need to write/generate your own test cases for benchmarking
EXEC_FLAGS := 0.001 0.00001 tests/s42-50.in

#-------------------------------------------------------------------------------
# Main Program
#-------------------------------------------------------------------------------

all: $(EXECS)

define make_target =
$(OBJ_DIR)/%.$(1).o: $$(subst $(OBJ_DIR)/,,%.cpp)
	@mkdir -p $$(dir $$@)
	$$(CXX) $$< -c $$(CXXFLAGS) $(CXXFLAGS_$(1)) -o $$@

protons_$(1): $(OBJECTS:.o=.$(1).o)
	@echo Linking $$@
	$$(CXX) $$^ $$(LDFLAGS) $(LDFLAGS_$(1)) -o $$@
	@echo

$(1): protons_$(1)

run_$(1): $(1)
	@echo
	./protons_$(1) $$(EXEC_FLAGS)

endef

# To debug the dynamically generated rules, replace eval with info
# $(foreach exec,$(EXECS),$(info $(call make_target,$(exec))))
$(foreach exec,$(EXECS),$(eval $(call make_target,$(exec))))

-include $(DEPENDS)

#-------------------------------------------------------------------------------
# Clean
#-------------------------------------------------------------------------------

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(foreach exec,$(EXECS),protons_$(exec))
