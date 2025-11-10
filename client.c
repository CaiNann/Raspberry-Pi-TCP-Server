#include <stdio.h>
#include "message_codes.h"
#include "client.h"
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>

#define BUF_SIZE 128
#define SERVER_ADDR "192.168.1.134"

int server_sig;

int main(void) {
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0) {
		perror("Socket failed");
		return 1;
	}

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	int	connection = connect(client_fd, (struct sockaddr*)&address, (socklen_t)sizeof(address));
	if (connection < 0) {
		perror("Socket failed");
		return 1;
	}
	printf("Successfully connected to server...\n");
	
	while (1) {
		ssize_t read_bytes = read(client_fd, &server_sig, sizeof(server_sig)); 
		server_sig = ntohl(server_sig);
		if (read_bytes < 0) {
			perror("Read from server failed");
			exit(1);
		}

		switch (server_sig) {
			case REQ_PASSWRD: {
				char passwrd[128];
				get_passwrd(passwrd, sizeof(passwrd));
				int exchange = pass_exchange(client_fd, passwrd);
				if (exchange == 0) {
					printf("Password accepted\n");
					write_to(STDOUT_FILENO, "File upload or download? (U/D): ");
					char buffer[BUF_SIZE];
					int read_bytes = read_from(STDIN_FILENO, buffer, BUF_SIZE); 
					if (buffer[0] == 'U') {
						init_file_upload();
						continue;
					}
					if (buffer[0] == 'D') {
						init_file_download();
						continue;
					}
					else {
						write_to(STDOUT_FILENO, "Invalid input\n");
					}
					continue;
				}
			}
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

int get_passwrd(char* buf, ssize_t size) {
	write_to(STDOUT_FILENO, "Password: ");
	int bytes_read = read_from(STDIN_FILENO, buf, size);
	buf[bytes_read - 1] = '\0';
	return 0;
}

int pass_exchange(int server_fd, char* passwrd) {

	int server_code = 0;

	if (write(server_fd, passwrd, strlen(passwrd)) < 0) {
		perror("Write failed");
		return 1;
	}
	
	read_from(server_fd, &server_code, sizeof(server_code));

	server_code = ntohl(server_code);

	if (server_code == ACCEPT_PASSWRD) {
		return 0;
	}
	if (server_code == CHARLIM_PASSWRD) {
		write_to(STDOUT_FILENO, "Password too long, try again\n");
		return 2;
	}
	if (server_code == DENY_PASSWRD) {
		write_to(STDOUT_FILENO, "Incorrect password, try again\n");
		return 3;
	}
	if (server_code == TIMEOUT_ERR) {
		write_to(STDOUT_FILENO, "Too many attempts...");
		exit(1);
	}
	return 4;
}

int write_to(int fd, char* string) {
	if (write(fd, string, strlen(string)) < 0) {
		perror("Write failed");
		exit(1);
	}
	return 0;
}

int send_code(int fd, int code) {
	code = htonl(code);
	if (write(fd, code, sizeof(code)) < 0) {
		perror("Write failed");
		exit(1);
	}
	return 0;
}

int read_from(int fd, char* buffer, size_t num_bytes) {
	int num_bytes_read = read(fd, buffer, num_bytes);
	if (num_bytes_read < 0) {
		perror("Read failed");
		exit(1);
	}
	return num_bytes_read;
}

int init_file_upload(int server_fd) {
	int server_code = NULL;
	send_code(server_fd, FILE_UPLOAD_REQ);

	int read_bytes = read_from(server_fd, &server_code, sizeof(server_code));
	server_code = ntohl(server_code);
	if (server_code == FILE_UPLOAD_ACK) {
		//
	}
}
