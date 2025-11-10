OPENSSL_PATH = /opt/homebrew/opt/openssl@3

CC = clang
FLAGS = -Wall -Wextra -Werror -pedantic
CPPFLAGS = -I$(OPENSSL_PATH)/include
LDFLAGS = -L$(OPENSSL_PATH)/lib
SERVER_TARGET = server
CLIENT_TARGET = client

all: $(SERVER_TARGET) $(CLIENT_TARGET)

server: server.o
	$(CC) $(FLAGS) $(CPPFLAGS) $(LDFLAGS) -o server server.o -lcrypto

server.o: server.c
	$(CC) $(FLAGS) $(CPPFLAGS) -c server.c -o server.o

client: client.o
	$(CC) $(FLAGS) -o client client.o

client.o: client.c
	$(CC) $(FLAGS) -c client.c -o client.o

clean:
	rm -f server.o client.o server client
