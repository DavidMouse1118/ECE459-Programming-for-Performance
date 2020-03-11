print-%: ; @echo $*=$($*)

.PHONY: all clean

#-------------------------------------------------------------------------------
# Test Cases
#-------------------------------------------------------------------------------

# Secrets are all 012345 (number indicates length of secret)
JWT_TOKEN_4 := eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.shNrMqeoWA3La5bOmJ9rzGtX8rh4M9fR93HVbE3JQTA 4 abcdefghijklmnopqrstuvwxyz0123456789
JWT_TOKEN_5 := eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.2fQveXQ-Af9-aI4lwINSON5WN1EyLDqFoHCxd3hnGjA 5 abcdefghijklmnopqrstuvwxyz0123456789
JWT_TOKEN_6 := eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.KQl9CGnVpjtdQ-nB_gV-EyjpVy8cwUzgVkbL8ErFUJE 6 abcdefghijklmnopqrstuvwxyz0123456789

EXEC_FLAGS := $(JWT_TOKEN_4)

#-------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------

OBJ_DIR :=./bin
SOURCES := $(shell find . -type f -name *.cpp)
OBJECTS := $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
DEPENDS := $(OBJECTS:.o=.d)

CXXFLAGS := -I./src -I./lib -MMD -MP -std=c++0x -g -Wall -Wno-sign-compare
LDFLAGS  :=
CXX      := g++

CXXFLAGS_ocl := -DUSE_OPENCL
LDFLAGS_ocl  := -lOpenCL

EXECS := ocl

#-------------------------------------------------------------------------------
# Main Program
#-------------------------------------------------------------------------------

all: $(EXECS)

define make_target =
$(OBJ_DIR)/%.$(1).o: $$(subst $(OBJ_DIR)/,,%.cpp)
	@mkdir -p $$(dir $$@)
	$$(CXX) $$< -c $$(CXXFLAGS) $(CXXFLAGS_$(1)) -o $$@

jwtcracker_$(1): $(OBJECTS:.o=.$(1).o)
	@echo Linking $$@
	$$(CXX) $$^ $$(LDFLAGS) $(LDFLAGS_$(1)) -o $$@
	@echo

$(1): jwtcracker_$(1)

run_$(1): $(1)
	@echo
	./jwtcracker_$(1) $$(EXEC_FLAGS)

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
	rm -f $(foreach exec,$(EXECS),jwtcracker_$(exec))
