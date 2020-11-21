#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>

#include "cson.h"


#ifdef __cplusplus
extern "C" {
#endif

int http_server(const int port);
int accept_client(const int servfd);
int recv_challenge(const int sockfd, cson_t *cson);
int send_response(const int sockfd, const char *data, const size_t len);

#ifdef __cplusplus
}
#endif

#endif
