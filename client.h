#ifndef CLIENT_H
#define CLIENT_H

int get_passwrd(char* buf, ssize_t size); 

int pass_exchange(int server_fd, char* passwrd);

int init_file_upload(int server_fd);

int write_to(int fd, char* string);

int send_code(int fd, int code);

int read_from(int fd, char* buffer, size_t num_bytes);

int get_code(int fd, int* buffer, size_t num_bytes);

#endif
