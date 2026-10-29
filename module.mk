BIN += stage-0/loki

STAGE_0_SRC := $(wildcard stage-0/*.c)
STAGE_0_SRC += stage-0/lex.c
STAGE_0_OBJ := $(STAGE_0_SRC:%.c=%.o)
STAGE_0_DEP := $(STAGE_0_SRC:%.c=%.d)

SRC += $(STAGE_0_SRC)
OBJ += $(STAGE_0_OBJ)
DEP += $(STAGE_0_DEP)

stage-0/lex.o: CFLAGS += -Wno-pedantic
stage-0/lex.c: RE2C_FLAGS += --no-generation-date

stage-0/loki: $(STAGE_0_OBJ)

clean-stage-0:
	$(RM) stage-0/loki $(STAGE_0_OBJ) stage-0/lex.c
