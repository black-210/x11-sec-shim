CC ?= cc
AR ?= ar
CPPFLAGS ?= -I.
BASE_CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic -fPIC
LDFLAGS ?=
LDLIBS ?=

ifeq ($(SANITIZE),1)
  CFLAGS := $(BASE_CFLAGS) -g -fsanitize=address,undefined
else
  CFLAGS := $(BASE_CFLAGS)
endif

CORE_SRC = x11_sec_shim.c x11_sec_health.c x11_sec_acl.c x11_sec_token.c x11_sec_rate.c x11_sec_audit.c \
	x11_sec_policy.c x11_sec_client.c x11_sec_socket.c x11_sec_event.c \
	x11_sec_logger.c x11_sec_runtime.c
CORE_OBJ = $(CORE_SRC:.c=.o)
LIB = libx11_sec_shim.a
TEST_SRC = x11_sec_shim_test.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_BIN = x11_sec_shim_test

.PHONY: all test check clean help

all: $(LIB) $(TEST_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

check: clean all test

$(LIB): $(CORE_OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(TEST_BIN): $(TEST_OBJ) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) $(TEST_OBJ) $(LIB) $(LDLIBS) -o $@

-include $(CORE_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

clean:
	rm -f $(CORE_OBJ) $(TEST_OBJ) $(CORE_OBJ:.o=.d) $(TEST_OBJ:.o=.d) $(LIB) $(TEST_BIN)

help:
	@printf '%s\n' 'make        build the library and tests' 'make test   run the regression suite' 'make check  clean, build, and test' 'make SANITIZE=1 check  enable ASan/UBSan'
