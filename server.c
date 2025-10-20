#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");  // Bind socket with
													   // localhost
	address.sin_port = htons(8080);        // Port 8080

	bind(server_fd, (struct sockaddr*)&address, (socklen_t)sizeof(address));

	listen(server_fd, 3);
	printf("Server is now listening on port %d...\n", address.sin_port); 

	for(;;) {
		int addrlength = sizeof(address);
		int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlength);
		printf("Connection accepted!");

		char buffer[1024] = {0};
		size_t bytes = read(client_fd, buffer, 1024);
		printf("%d bytes read from client: %s\n", bytes, buffer);

		close(client_fd);
	}

	close(server_fd);

	return 0;
}
