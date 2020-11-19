#include "server.h"

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cson.h"


int http_server(const int port) {
    const int servfd = socket(AF_INET, SOCK_STREAM, 0);
    if (servfd < 0) {
        return -1;
    }

    const int on = 1;
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
        return -1;
    }

    if (listen(servfd, 10) < 0) {
        close(servfd);
        return -1;
    }

    return servfd;
}

int accept_client(const int servfd) {
    const int sockfd = accept(servfd, NULL, NULL);
    if (sockfd < 0) {
        return -1;
    }

    const int on = 1;
    // Make sure CORK is on, NODELAY should be inherited, but CORK is unclear
    setsockopt(sockfd, SOL_TCP, TCP_CORK, &on, sizeof(int));

    return sockfd;
}

void sflush(const int sockfd) {
    // Because we have NODELAY, toggling CORK will force flush any partial
    // frame in the kernel net buffer.
    const int off = 0;
    setsockopt(sockfd, SOL_TCP, TCP_CORK, &off, sizeof(int));
    const int on = 1;
    setsockopt(sockfd, SOL_TCP, TCP_CORK, &on, sizeof(int));
}

void send_pong(const int sockfd) {
    // Canned empty OK reply
    const char reply[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    send(sockfd, reply, sizeof(reply) - 1, 0);
    sflush(sockfd);
}

void send_bad_request(const int sockfd) {
    // Canned  Bad Request reply
    const char reply[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    send(sockfd, reply, sizeof(reply) - 1, 0);
    sflush(sockfd);
}

// TODO: This is shitty, we can parse in the data buffer and realloc to save on recv's
ssize_t recvline(const int sockfd, char *buf, const size_t len) {
    char c;
    size_t i = 0;
    while (i < len) {
        ssize_t n = recv(sockfd, &c, 1, 0);
        if (n < 0) {
            return -1;
        } else if (n > 0) {
            if ((buf[i++] = c) == '\n') {
                break;
            }
        } else {
            break;
        }
    }
    return i;
}

int recv_challenge(const int sockfd, cson_t *cson) {
    // NOTE: Welcome to Assumption-Land(tm)

    // Deal with the request line
    ssize_t n = 0;
    char buf[2048];
    if ((n = recvline(sockfd, buf, sizeof(buf))) <= 0) {
        return -1;
    }

    // Assume GET, handle the liveness check immediately
    if (buf[0] == 'G') {
        send_pong(sockfd);
        return -1;
    }

    // Assume challenge request starting here

    // Consume headers and "parse" the Content-Length
    size_t datalen = 0;
    for (;;) {
        if ((n = recvline(sockfd, buf, sizeof(buf))) <= 0) {
            return -1;
        }
        if (n > 14 && buf[0] == 'C' && buf[7] == '-' && buf[8] == 'L') {
            datalen = 0;
            char *b = buf + 16; // Skip the "Content-Length: " part to jump to value
            for (char c = *b; b - buf < n && (c >= '0' && c <= '9'); c = *(++b)) {
                datalen = datalen * 10 + (c - '0');
            }
        } else if (n == 2 && buf[0] == '\r' && buf[1] == '\n') {
            break;
        }
    }
    if (datalen <= 0 || datalen > 10 * 1024 * 1024 /* 10MB */) {
        send_bad_request(sockfd);
        return -1;
    }

    char* data = malloc(datalen);
    if (data == NULL) {
        exit(1);
    }
    char *p = data, *d = data;
    cson_init(cson);
    while (datalen > 0) {
        if ((n = recv(sockfd, d, datalen, 0)) < 0) {
            send_bad_request(sockfd);
            cson_free(cson);
            free(data);
            return -1;
        }
        datalen -= n;
        d += n;
        p += cson_update(cson, p, d - p);
    }

    free(data);

    return 0;
}

int send_response(const int sockfd, const char *data, const size_t len) {
    char headers[512];
    ssize_t n = sprintf(headers, "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: application/json\r\nContent-Length: %ld\r\n\r\n", len);
    if (n < 0) {
        return -1;
    }
    if (send(sockfd, headers, n, 0) < 0) {
        return -1;
    }
    if (send(sockfd, data, len, 0) < 0) {
        return -1;
    }
    sflush(sockfd);
    return 0;
}

int send_response_headers(const int sockfd) {
    const char headers[] = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: application/json\r\nTransfer-Encoding: chunked\r\n\r\n";
    if (send(sockfd, headers, sizeof(headers) - 1, 0) < 0) {
        return -1;
    }
    sflush(sockfd);
    return 0;
}

int send_response_chunk(const int sockfd, const char *data, const size_t len) {
    if (len == 0) {
        send(sockfd, "0\r\n\r\n", 5, 0);
    } else {
        char chunk_size[64];
        ssize_t n = sprintf(chunk_size, "%lX\r\n", len);
        if (n < 0) {
            return -1;
        }
        if (send(sockfd, chunk_size, n, 0) < 0) {
            return -1;
        }
        if (send(sockfd, data, len, 0) < 0) {
            return -1;
        }
        if (send(sockfd, "\r\n", 2, 0) < 0) {
            return -1;
        }
    }
    sflush(sockfd);
    return 0;
}
