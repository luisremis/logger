#ifndef CONNECTIONS_H
#define CONNECTIONS_H

int open_socket(int port);
int listen_socket(int listeFd);

/*Client operation */
int connect_to_server(const char* add, int port, int* connectionFd);

#endif