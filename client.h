#ifndef CLIENT_H
#define CLIENT_H

int get_passwrd(char* buf, ssize_t size); 

int pass_exchange(int server_fd, char* passwrd);

#endif
