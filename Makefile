CC = clang
FLAGS = -Wall -Wextra -Werror -pedantic
SERVER_TARGET = server
CLIENT_TARGET = client

# Detect platform
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	# macOS (Homebrew)
	OPENSSL_PATH := $(shell brew --prefix openssl@3)
	CPPFLAGS := -I$(OPENSSL_PATH)/include
	LDFLAGS := -L$(OPENSSL_PATH)/lib
else
	# Linux (Raspberry Pi / Ubuntu / Debian)
	CPPFLAGS :=
	LDFLAGS :=
endif

LDLIBS = -lcrypto

all: $(SERVER_TARGET) $(CLIENT_TARGET)

server: server.o
	$(CC) $(FLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

server.o: server.c
	$(CC) $(FLAGS) $(CPPFLAGS) -c $< -o $@

client: client.o
	$(CC) $(FLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

client.o: client.c
	$(CC) $(FLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f server.o client.o server client

