#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>

#include "cson.h"


#ifdef __cplusplus
extern "C" {
#endif

int http_server(const int port);
int accept_client(const int servfd);
int recv_challenge(const int sockfd, const char** chal, size_t* chal_ken);
int send_response(const int sockfd, const char *data, const size_t len);
int send_response_headers(const int sockfd);
int send_response_chunk(const int sockfd, const char *data, const size_t len);

#ifdef __cplusplus
}
#endif

#endif
