CC = cc
CPPFLAGS = -I.
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -fPIC

CORE_SRC = x11_sec_shim.c x11_sec_acl.c x11_sec_token.c x11_sec_rate.c x11_sec_audit.c \
	x11_sec_policy.c x11_sec_client.c x11_sec_socket.c x11_sec_event.c \
	x11_sec_logger.c x11_sec_runtime.c
CORE_OBJ = $(CORE_SRC:.c=.o)
LIB = libx11_sec_shim.a
TEST_SRC = x11_sec_shim_test.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_BIN = x11_sec_shim_test

all: $(LIB) $(TEST_BIN)

$(LIB): $(CORE_OBJ)
	ar rcs $@ $^

%.o: %.c %.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJ) $(LIB)
	$(CC) $(CFLAGS) $(TEST_OBJ) $(LIB) -o $@

clean:
	rm -f $(CORE_OBJ) $(TEST_OBJ) $(LIB) $(TEST_BIN)

.PHONY: all clean
