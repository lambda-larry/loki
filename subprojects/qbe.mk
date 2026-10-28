export PATH := ${PWD}/subprojects/qbe:${PATH}

subprojects/qbe/%.o: CC       := gcc
subprojects/qbe/%.o: CFLAGS   := -std=c99 -pedantic
subprojects/qbe/%.o: CFLAGS   += -O2
subprojects/qbe/%.o: CPPFLAGS += -MMD

subprojects/qbe/qbe: LDFLAGS += -static

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

subprojects/qbe/qbe: $(patsubst %.c, %.o, $(wildcard subprojects/qbe/*.c))
subprojects/qbe/qbe: $(patsubst %.c, %.o, $(wildcard subprojects/qbe/amd64/*.c))
subprojects/qbe/qbe: $(patsubst %.c, %.o, $(wildcard subprojects/qbe/arm64/*.c))
subprojects/qbe/qbe: $(patsubst %.c, %.o, $(wildcard subprojects/qbe/rv64/*.c))

subprojects/qbe/qbe:
	$(CC) $(LDFLAGS) $^ -o $@

subprojects/qbe/config.h:
	@touch $@

subprojects/qbe/main.o: subprojects/qbe/config.h

build-qbe: subprojects/qbe/qbe

clean-qbe:
	$(RM) subprojects/qbe/*.o subprojects/qbe/*/*.o subprojects/qbe/qbe

clean-all: clean-qbe

