# Author: A. Lochmann C 2010
# Based on a makefile found at: http://ubuntuforums.org/showthread.php?t=1204739
INCLUDE_PATHS=-I.
BUILD_PATH:=build
LD_TEXT = -e "LD\t$@"
LD_SO_TEXT = -e "LD SHARED\t$@"
CC_TEXT = -e "CC\t$<"
CXX_TEXT = -e "CXX\t$<"
DEP_TEXT= -e "DEP\t$<"

# FIND EVERY EXISTING DEPENDENCY FILE
EXISTING_DEPS:=$(shell find . -name '*.d')

ifndef V
V = 0
endif
ifeq ($(V),0)
OUTPUT= @
else
OUTPUT=
endif

SRC_CXX=wichteln.cc
SRC_C=smtpsocket.c quickmail.c
OBJ_CXX=$(SRC_CXX:%.cc=%.o)
OBJ_C=$(SRC_C:%.c=%.o)
OBJ=$(OBJ_C) $(OBJ_CXX)

WICHTELOMAT_BIN=wichtelomat

#***************************** COMMANDS AND FLAGS *****************************
# COMPILER AND LINKER FLAGS
CC:=gcc
C_FLAGS := -O3 -Wall -Werror -c -g $(INCLUDE_PATHS)
CXX:=g++
CXX_FLAGS:= -O3 -Wall -Werror -c -g -std=c++11 $(INCLUDE_PATHS)
CXX_DEP_FLAGS:= -O3 -std=c++11 $(INCLUDE_PATHS)
LD:=gcc
LD_FLAGS :=
LD_LIBS := -lcurl

#***************************** DO NOT EDIT BELOW THIS LINE EXCEPT YOU WANT TO ADD A TEST APPLICATION (OR YOU KNOW WHAT YOU'RE DOING :-) )***************************** 
DEP = $(subst .o,.d,$(OBJ))

all: $(DEP) $(WICHTELOMAT_BIN)

echo:
	@echo $(DEP)
	@echo $(OBJ)

$(WICHTELOMAT_BIN): $(OBJ)
	@echo $(LD_TEXT)
	$(OUTPUT)$(CXX) $^ $(LD_FLAGS)  $(LD_LIBS) -o $@

# C-specific targets
%.o: %.c %.d
	@echo $(CC_TEXT)
	$(OUTPUT)$(CC) $(C_FLAGS) $< -o $@

# Every dependency file depends only on the corresponding source file -- C files
%.d: %.c
	@echo $(DEP_TEXT)
	$(OUTPUT)$(call make-depend,$<,$(subst .d,.o,$@),$(subst .o,.d,$@))

# CXX-specific targets
%.o: %.cc %.d
	@echo $(CXX_TEXT)
	$(OUTPUT)$(CXX) $(CXX_FLAGS) $< -o $@

# Every dependency file depends only on the corresponding source file -- CXX files
%.d: %.cc
	@echo $(DEP_TEXT)
	$(OUTPUT)$(call make-depend,$<,$(subst .d,.o,$@),$(subst .o,.d,$@))

clean: clean-dep clean-obj

clean-dep:
	$(RM) $(DEP)

clean-obj:
	$(RM) $(OBJ)

#***************************** INCLUDE EVERY EXISTING DEPENDENCY FILE  *****************************
-include $(EXISTING_DEPS)
#*****************************		END INCLUDE		       *****************************

.PHONY: all clean clean-deps clean-obj

# usage: $(call make-depend,source-file,object-file,depend-file)
define make-depend
  $(CXX) -MM       \
        -MF $3    \
        -MP       \
        -MT $2    \
	$(CXX_DEP_FLAGS) \
        $1
endef

