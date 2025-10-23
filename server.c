#include <sys/socket.h>
#include "server.h"
#include "message_codes.h"
#include <openssl/sha.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {

	int server_fd = start_server();

	for(;;) {
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);

		int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd < 0) {
 	    	perror("accept failed");
    		exit(1);
		}

		if (check_password(client_fd) == 0) {
			printf("Unauthorized user, closing connection...");
			close(client_fd);
			continue;
		}

		printf("Connection accepted!\n");

		char buffer[1024] = {0};
		size_t bytes = read(client_fd, buffer, 1024);
		if (bytes < 0) {
			perror("Read failed");
			exit(1);
		}

		printf("%d bytes read from client: %s\n", bytes, buffer);

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

int check_password(int client_fd) {
	int retry = 0;
	int pass_file = open("pass.txt", O_RDONLY);
	char buffer[65];
	memset(buffer, 0, sizoef(buffer));
	while (retry != 3) {
		if (write(client_fd, &REQ_PASSWRD, sizeof(REQ_PASSWORD)) < 0) {
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
			if (write(client_fd, &CHARLIM_PASSWRD, sizeof(CHARLIM_PASSWRD)) < 0) {
				perror("Writing error failed");
				exit(1);
			}
			continue;
		}
		buffer[pass_read] = '\0';
		if (match_passwrd(pass_file, buffer)) {
			if (write(client_fd, &ACCEPT_PASSWRD, sizeof(ACCEPT_PASSWRD)) < 0) {
				perror("Writing error failed");
				exit(1);
			}
			return 1;
		} else {
			if (write(client_fd, &DENY_PASSWRD, sizeof(DENY_PASSWRD)) < 0) {
				perror("Writing error failed");
				exit(1);
			}
			return 0;
		}
	}
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
        if (strcmp(line, buffer) == 0) {
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
