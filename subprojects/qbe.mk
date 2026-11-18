export PATH := ${PWD}/subprojects/qbe:${PATH}

QBE := subprojects/qbe/qbe

subprojects/qbe/%.o: CC       := gcc
subprojects/qbe/%.o: CFLAGS   := -std=c99 -pedantic
subprojects/qbe/%.o: CFLAGS   += -O2
subprojects/qbe/%.o: CPPFLAGS += -MMD

${QBE}: LDFLAGS += -static

ifeq ($(shell uname -s),Darwin)
ifneq ($(filter arm%,$(shell uname -p)),)
subprojects/qbe/%.o: CPPFLAGS += -DDeftgt=T_arm64_apple
endif
ifeq ($(shell uname -m),x86_64)
subprojects/qbe/%.o: CPPFLAGS += -DDeftgt=T_amd64_apple
endif
else
ifeq ($(shell uname -m),x86_64)
subprojects/qbe/%.o: CPPFLAGS += -DDeftgt=T_amd64_sysv
endif
ifneq ($(filter arm%,$(shell uname -m)),)
subprojects/qbe/%.o: CPPFLAGS += -DDeftgt=T_arm64
endif
endif

-include $(wildcard subprojects/qbe/*.d)
-include $(wildcard subprojects/qbe/*/*.d)

${QBE}: $(patsubst %.c, %.o, $(wildcard subprojects/qbe/*.c))
${QBE}: $(patsubst %.c, %.o, $(wildcard subprojects/qbe/amd64/*.c))
${QBE}: $(patsubst %.c, %.o, $(wildcard subprojects/qbe/arm64/*.c))
${QBE}: $(patsubst %.c, %.o, $(wildcard subprojects/qbe/rv64/*.c))

${QBE}:
	$(CC) $(LDFLAGS) $^ -o $@

subprojects/qbe/config.h:
	@touch $@

subprojects/qbe/main.o: subprojects/qbe/config.h

build-qbe: ${QBE}

clean-qbe:
	$(RM) subprojects/qbe/*.o subprojects/qbe/*/*.o ${QBE}

clean-all: clean-qbe

