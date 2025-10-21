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
	printf("argc: %d", argc);
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
	printf("Connect return code: %d", connection);
	printf("Error code: %s (errno: %d)", strerror(errno), errno);
	printf("Successfully connected to server...\n");

	if (argc == 1) {
		printf("Writing 'Hello Server!'");
		char *message = "Hello Server!\n";
		write(client_fd, message, strlen(message));
	} else {
		for (int i = 1; i < argc; i++) {
			char buffer[1024];
			memset(&buffer, 0, sizeof(buffer));
			int input_fd = open(args[i], O_RDONLY);
			size_t r_bytes;
			while ((r_bytes = read(input_fd, buffer, (size_t)1024)) != 0) {
				printf("Read %zu bytes from %s\n", r_bytes, args[i]);
				int w_bytes = write(client_fd, buffer, r_bytes);
				printf("Wrote %d bytes to server\n", w_bytes);
			}
			close(input_fd);
		}
	}

	close(client_fd);
	printf("Closed connection to server.\n");

	return 0;
}
