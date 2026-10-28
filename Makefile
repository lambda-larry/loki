## lambda-larry build system
##
## ::toc_begin
## - Tools ............................................................15. [XTO]
## - Compiler flags ...................................................42. [XCF]
## - Build configuration ..............................................26. [XBM]
## - Build rules ......................................................24. [XBR]
## - Misc .............................................................15. [XMS]
## - Clean ............................................................26. [XBC]
## ::toc_end

################################################################################
## Tools                                                                [XTO] ##
################################################################################

CC := gcc
CXX := g++

CTAGS := ctags
CTAGS_FLAGS := -a
CSCOPE := cscope
CSCOPE_FLAGS := -bqvR

CLANG_FORMAT := clang-format


################################################################################
## Compiler flags                                                       [XCF] ##
################################################################################

CFLAGS += -std=c23 -Wall -pedantic -Wextra -Wpedantic
CXXFLAGS += -std=c++20 -Wall -pedantic -Wextra -Wpedantic

-include config.mk
-include local.mk

CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-label

CXXFLAGS += -Wno-unused-parameter
CXXFLAGS += -Wno-unused-function
CXXFLAGS += -Wno-unused-label

CFLAGS += -Wnonnull

CFLAGS += -fPIC
CXXFLAGS += -fPIC

CXXFLAGS += -fno-exceptions -fno-rtti

ifeq ($(origin FAST_MATH),undefined)
else
CFLAGS += -ffast-math
CFLAGS += -fassociative-math
CFLAGS += -freciprocal-math
CFLAGS += -fno-signed-zeros

CXXFLAGS += -ffast-math
CXXFLAGS += -fassociative-math
CXXFLAGS += -freciprocal-math
CXXFLAGS += -fno-signed-zeros

# Because of -ffast-math, NaN and Infinity will never happen
CPPFLAGS += -D__builtin_isinf_sign\(...\)=0
CPPFLAGS += -D__builtin_isnan\(...\)=0
endif

################################################################################
## Build configuration                                                  [XBM] ##
################################################################################

.PHONY: default debug release

default: debug

debug: CFLAGS += -Og
debug: CFLAGS += -g3 -ggdb3
debug: all

release: CFLAGS += -O2
release: CPPFLAGS += -DNDEBUG
release: all

address-sanitizer: CFLAGS += -fsanitize=address
address-sanitizer: debug

MODULES := subprojects tools .

-include $(MODULES:%=%/module.mk)

shell:
	@exec bash

################################################################################
## Build rules                                                          [XBR] ##
################################################################################

.PHONY: all

all: $(BIN) $(SUB_BIN)

-include $(DEP) $(SUB_DEP)

%.d: %.c
	@$(CC) $(CPPFLAGS) -MM -MG -MT 'tags cscope.out $(@:.d=.o) $@' $< -MF $@

%.d: %.cpp
	@$(CXX) $(CPPFLAGS) -MM -MG -MT 'tags cscope.out $(@:.d=.o) $@' $< -MF $@

(%): %;
%.a:
	$(AR) $(ARFLAGS) $@ $?

%.so: LDFLAGS += -shared
%.so:
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

################################################################################
## Misc                                                                 [XMS] ##
################################################################################

tags:
	$(CTAGS) $(CTAGS_FLAGS) $?

cscope.out:
	$(CSCOPE) $(CSCOPE_FLAGS) $?

.PHONY: clang-format

clang-format: $(SRC)
	$(CLANG_FORMAT) -i $?

################################################################################
## Clean                                                                [XBC] ##
################################################################################

.PHONY: clean-all clean-bin clean-obj clean-dep clean-sub clean-tags clean-cscope

clean-all: clean-bin clean-obj clean-dep clean-sub clean-tags clean-cscope

clean: clean-bin clean-obj

clean-bin:
	$(RM) $(BIN)

clean-obj:
	$(RM) $(OBJ)

clean-dep:
	$(RM) $(DEP) $(SUB_DEP)

clean-sub:
	$(RM) $(SUB_OBJ) $(SUB_BIN)

clean-tags:
	$(RM) tags

clean-cscope:
	$(RM) cscope.*
