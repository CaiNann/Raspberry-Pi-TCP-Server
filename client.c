#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>

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

	if (argc == 1) {
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

	close(client_fd);
	printf("Closed connection to server.\n");

	return 0;
}
