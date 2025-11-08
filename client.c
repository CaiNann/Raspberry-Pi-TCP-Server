#include <stdio.h>
#include "message_codes.h"
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>

#define BUF_SIZE 128

int server_sig;

int main(int argc, char *args[]) {
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0) {
		perror("Socket failed");
		return 1;
	}

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = inet_addr("192.168.1.134");

	int	connection = connect(client_fd, (struct sockaddr*)&address, (socklen_t)sizeof(address));
	if (connection < 0) {
		perror("Socket failed");
		return 1;
	}
	printf("Successfully connected to server...\n");
	
	while (1) {
		ssize_t read_bytes = read(client_fd, &server_sig, sizeof(server_sig)); 
		if (read_bytes < 0) {
			perror("Read from server failed");
			exit(1);
		}

		switch (server_sig) {
			case REQ_PASSWRD:
				char* passwrd = get_passwrd();
				int exchange = pass_exchange(client_fd, passwrd);
				if (exchange == 0) {
					printf("Password accepted\n");
					close(client_fd);
					return 0;
				}
		}

	}

/*	if (argc == 1) {
		char *message = "Hello Server!\n";
		size_t w_bytes = write(client_fd, message, strlen(message));

		if (w_bytes < 0) {
			perror("Write failed");
			return 1;
		}
	} else {
		for (int i = 1; i < argc; i++) {
			char buffer[1024];
			memset(&buffer, 0, sizeof(buffer));

			int input_fd = open(args[i], O_RDONLY);
			if (input_fd < 0) {
				perror("Failed to open file");
				return 1;
			}

			size_t r_bytes;
			while ((r_bytes = read(input_fd, buffer, (size_t)1024)) != 0) {
				if (r_bytes < 0) {
					perror("Failed to read from file");
					return 1;
				}

				printf("Read %zu bytes from %s\n", r_bytes, args[i]);

				int w_bytes = write(client_fd, buffer, r_bytes);
				if (w_bytes < 0) {
					perror("Failed to write buffer to server");
					return 1;
				}

				printf("Wrote %d bytes to server\n", w_bytes);
			}
			close(input_fd);
		}
	}
	*/

	char* get_passwrd(void) {
		if (write(STDOUT_FILENO, "Password: ", 10) < 0) {
			perror("Write failed");
			return NULL;
		}
		char passwrd[128];
		memset(passwrd, 0, 128);
		int bytes_read = read(STDIN_FILENO, passwrd, sizeof(passwrd))
		if (bytes_read < 0) {
			perror("Read failed");
			return NULL;
		}
		passwrd[bytes_read] = '\0';
		return passwrd;
	}

	int pass_exchange(int server_fd, char* passwrd) {

		int server_code = 0;

		if (write(server_fd, passwrd, sizeof(passwrd)) < 0) {
			perror("Write failed");
			return 1;
		}
		
		if (read(server_fd, &server_code, sizeof(server_code)) < 0) {
			perror("Read failed");
			return 1;
		}

		if (server_code == ACCEPT_PASSWRD) {
			return 0;
		}
		if (server_code == CHARLIM_PASSWRD) {
			if (write(STDOUT_FILENO, "Password too long, try again\n", 29) < 0) {
				perror("Write failed");
				return 1;
			}
			return 2;
		}
		if (server_code == DENY_PASSWRD) {
			if (write(STDOUT_FILENO, "Incorrect password, try again\n", 30) < 0) {
				perror("Write failed");
				return 1;
			}
			return 3;
		}
		if (server_code == TIMEOUT_ERR) {
			if (write(STDOUT_FILENO, "Too many attempts...", 20) < 0) {
				perror("Write failed");
				return 1;
			}
			exit(1);
		}
	}
}
