#include "server.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

int http_server(int port) {
  int servfd = socket(AF_INET, SOCK_STREAM, 0);
  if (servfd < 0) {
    fprintf(stderr, "Failed to create socket\n");
    return -1;
  }

  int on = 1;
  // Try to re-use port if already bound
  setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
  // Disable Nagle's algorithm, do not wait for ACK to send
  setsockopt(servfd, SOL_TCP, TCP_NODELAY, &on, sizeof(int));
  // Allow us to decide when to send partial frames by buffering until uncorked
  setsockopt(servfd, SOL_TCP, TCP_CORK, &on, sizeof(int));
  // "Accept" client connection only on first packet, aka first request
  setsockopt(servfd, SOL_TCP, TCP_DEFER_ACCEPT, &on, sizeof(int));
  // Send ACKs fast (tm)
  setsockopt(servfd, SOL_TCP, TCP_QUICKACK, &on, sizeof(int));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(servfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    close(servfd);
    fprintf(stderr, "Failed to bind to %s:%d\n", inet_ntoa(addr.sin_addr), port);
    return -1;
  }

  if (listen(servfd, 10) < 0) {
    close(servfd);
    fprintf(stderr, "Failed to listen to %s:%d\n", inet_ntoa(addr.sin_addr), port);
    return -1;
  }

  printf("Listening on %s:%d\n", inet_ntoa(addr.sin_addr), port);

  return servfd;
}

int accept_client(int servfd) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  socklen_t addrlen = 0;
  int sockfd = accept(servfd, (struct sockaddr*)&addr, &addrlen);
  if (sockfd < 0) {
    fprintf(stderr, "Failed to accept client\n");
    return -1;
  }

  printf("Client connected %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

  int on = 1;
  // Make sure CORK is on, NODELAY should be inherited, but CORK is unclear
  setsockopt(sockfd, SOL_TCP, TCP_CORK, &on, sizeof(int));

  return sockfd;
}

void sflush(int sockfd) {
  // Because we have NODELAY, toggling CORK will force flush any partial frame
  // in the kernel net buffer.
  int off = 0;
  setsockopt(sockfd, SOL_TCP, TCP_CORK, &off, sizeof(int));
  int on = 1;
  setsockopt(sockfd, SOL_TCP, TCP_CORK, &on, sizeof(int));
}
