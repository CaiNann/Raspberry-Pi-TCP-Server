#ifndef SERVER_H
#define SERVER_H

// Function declarations (prototypes)
int start_server(void);

int check_passwrd(int client_fd);

int match_passwrd(int fd, char *passwrd);

#endif

