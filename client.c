#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>

int main(void) {
	int client_fd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(client_fd, (struct sockaddr*)&address, (socklen_t)sizeof(address));
	printf("Successfully connected to server...\n");

	char *message = "Hello Server!\n";
	write(client_fd, message, strlen(message));

	close(client_fd);
	printf("Closed connection to server.\n");

	return 0;
}
