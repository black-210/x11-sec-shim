CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -fPIC
CPPFLAGS = -I.

LIB = libx11_sec_shim.a
OBJ = x11_sec_shim.o
TEST_OBJ = x11_sec_shim_test.o
TEST_BIN = x11_sec_shim_test

all: $(LIB) $(TEST_BIN)

$(LIB): $(OBJ)
	ar rcs $@ $^

$(OBJ): x11_sec_shim.c x11_sec_shim.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c x11_sec_shim.c -o $@

$(TEST_OBJ): x11_sec_shim_test.c x11_sec_shim.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c x11_sec_shim_test.c -o $@

$(TEST_BIN): $(TEST_OBJ) $(LIB)
	$(CC) $(CFLAGS) $(TEST_OBJ) $(LIB) -o $@

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(LIB) $(TEST_BIN)

.PHONY: all clean
