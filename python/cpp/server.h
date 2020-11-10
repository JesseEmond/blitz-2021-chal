#ifndef SERVER_H
#define SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

int http_server(int port);
int accept_client(int servfd);
void reply_ping(int sockfd);

#ifdef __cplusplus
}
#endif

#endif
