#include <sys/socket.h>
#include <fcntl.h>
#include "server.h"
#include "message_codes.h"
#include <openssl/sha.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 128

int main(void) {
	int status_code = 0;

	int server_fd = start_server();

	for(;;) {
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);

		int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd < 0) {
 	    	perror("accept failed");
    		exit(1);
		}

		if (check_passwrd(client_fd) == 1) {
			printf("Unauthorized user, closing connection...");
			close(client_fd);
			continue;
		}

		printf("Connection accepted!\n");

		if (read(client_fd, &status_code, sizeof(status_code)) < 0) {
			perror("Read failed");
			exit(1);
		}

		char buffer[1028];
		int FILE_UPLOAD_ACK = htonl(FILE_UPLOAD_ACK);

		if (ntohl(status_code) == FILE_UPLOAD_REQ) {
			write(client_fd, FILE_UPLOAD_ACK, sizeof(FILE_UPLOAD_ACK));
			char filename[1028];
			int read_bytes = read(client_fd, filename, sizeof(filename));
			if (read_bytes < 0) {
				perror("Read failed");
				exit(1);
			}
			int new_file = open(filename, O_WRONLY | O_CREAT);
			while (1) {
				int buffer_bytes = read(client_fd, buffer, sizeof(buffer));
				if (strcmp(buffer, "UF") == 0) {
					break;
				}
				write(new_file, buffer, buffer_bytes); 
			}
		}

		close(client_fd);
	}

	close(server_fd);

	return 0;
}

int start_server(void) {
	
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("Server socket failed");
		exit(1);
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;  // Bind socket with
	address.sin_port = htons(8080);        // Port 8080

	int	addr_bind = bind(server_fd, (struct sockaddr*)&address, (socklen_t)sizeof(address));
	if (addr_bind < 0) {
		perror("bind failed");
		exit(1);
	}

	int listener = listen(server_fd, 3);
	if (listener < 0) {
		perror("Listen failed");
		exit(1);
	}

	printf("Server is now listening on port %d...\n", ntohs(address.sin_port)); 
	return server_fd;
}

int check_passwrd(int client_fd) {
	int req_passwrd = htonl(REQ_PASSWRD);
	int accept_passwrd = htonl(ACCEPT_PASSWRD);
	int charlim_passwrd = htonl(CHARLIM_PASSWRD);
	int deny_passwrd = htonl(DENY_PASSWRD);
	int timeout_err = htonl(TIMEOUT_ERR);

	int retry = 0;
	int pass_file = open("pass.txt", O_RDONLY);
	char buffer[65];
	memset(buffer, 0, sizeof(buffer));
	while (retry != 3) {
		if (write(client_fd, &req_passwrd, sizeof(req_passwrd)) < 0) {
			perror("Failed to request password");
			exit(1);
		}
		int pass_read = read(client_fd, buffer, 65);
		if (pass_read < 0) {
			perror("Reading password failed");
			exit(1);
		}
		if (pass_read == 65) {
			retry++;
			if (write(client_fd, &charlim_passwrd, sizeof(charlim_passwrd)) < 0) {
				perror("Writing error failed");
				exit(1);
			}
			continue;
		}
		if (match_passwrd(pass_file, buffer)) {
			if (write(client_fd, &accept_passwrd, sizeof(accept_passwrd)) < 0) {
				perror("Writing error failed");
				exit(1);
			}
			close(pass_file);
			return 0;
		} else {
			if (write(client_fd, &deny_passwrd, sizeof(deny_passwrd)) < 0) {
				perror("Writing error failed");
				exit(1);
			}
			retry++;
			continue;
		}
	}
	if (write(client_fd, &timeout_err, sizeof(timeout_err)) < 0) {
		perror("Writing error failed");
		exit(1);
	}
	close(pass_file);
	return 1;
}

int match_passwrd(int fd, char *passwrd) {

	char read_buf[BUF_SIZE];
    char line[BUF_SIZE];
    ssize_t bytes_read;
    size_t line_len = 0;

    while ((bytes_read = read(fd, read_buf, sizeof(read_buf))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            char c = read_buf[i];

            if (c == '\n' || c == '\r') {
                // terminate current line
                line[line_len] = '\0';

                if (strcmp(line, passwrd) == 0) {
                    close(fd);
                    return 1; // match found
                }

                line_len = 0; // reset for next line
            } else {
                if (line_len < sizeof(line) - 1) {
                    line[line_len++] = c;
                } else {
                    fprintf(stderr, "Line too long in pass.txt\n");
                    close(fd);
                    return -1;
                }
            }
        }
    }

    // Check last line if file doesn't end with newline
    if (line_len > 0) {
        line[line_len] = '\0';
        if (strcmp(line, passwrd) == 0) {
            close(fd);
            return 1;
        }
    }

    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return -1;
    }

    close(fd);
    return 0; // no match
}
