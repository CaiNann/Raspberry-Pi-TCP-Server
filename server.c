#include <sys/socket.h>
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
    		return 1;
		}

		check_password();

		printf("Connection accepted!\n");

		char buffer[1024] = {0};
		size_t bytes = read(client_fd, buffer, 1024);
		if (bytes < 0) {
			perror("Read failed");
			return 1;
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
		return 1;
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;  // Bind socket with
	address.sin_port = htons(8080);        // Port 8080

	int	addr_bind = bind(server_fd, (struct sockaddr*)&address, (socklen_t)sizeof(address));
	if (addr_bind < 0) {
		perror("bind failed");
		return 1;
	}

	int listener = listen(server_fd, 3);
	if (listener < 0) {
		perror("Listen failed");
		return 1;
	}

	printf("Server is now listening on port %d...\n", ntohs(address.sin_port)); 
	return server_fd;
}

int check_password(char* password) {
	int pass_file = open("pass.txt", O_RDONLY);
}
