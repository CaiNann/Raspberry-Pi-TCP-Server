#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>

int main(int argc, char *args[]) {
	int client_fd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(client_fd, (struct sockaddr*)&address, (socklen_t)sizeof(address));
	printf("Successfully connected to server...\n");

	if (argc == 1) {
		char *message = "Hello Server!\n";
		write(client_fd, message, strlen(message));
	} else {
		for (int i = 1; i < argc; i++) {
			char buffer[1024];
			memset(&buffer, 0, sizeof(buffer));
			int input_fd = open(args[i], O_RDONLY);
			while (size_t r_bytes = read(input_fd, buffer, (size_t)1024) != 0) {
				printf("Read %d bytes from %s\n", r_bytes, args[i]);
				int w_bytes = write(client_fd, buffer, bytes);
				printf("Wrote %d bytes to server\n", w_bytes);
			}
			close(input_fd);
		}
	}

	close(client_fd);
	printf("Closed connection to server.\n");

	return 0;
}
