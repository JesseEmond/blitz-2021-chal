#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int http_server(const int port);
int accept_client(const int servfd);
ssize_t recv_challenge(const int sockfd, char **data);
int send_response(const int sockfd, const char *data, const size_t len);

#ifdef __cplusplus
}
#endif

#endif
