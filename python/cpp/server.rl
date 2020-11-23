#include "server.h"

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cson.h"

%%{
    machine http_request;

    action handle_get {
        send_pong(sockfd);
        free(buffer);
        return -1;
    }

    action content_length {
        bodylen *= 10;
        bodylen += fc - '0';
    }

    CRLF = "\r\n";

    method = "GET" %handle_get | "POST";

    header = ( ( "Content-Length" ":" " "* digit+ $content_length )
             | ( print+ )
             ) :> CRLF;

    request_line = method " " print+ CRLF;

    request = request_line header+ :> CRLF;

    main := request @{ fbreak; };

    write data noerror nofinal noentry;
}%%

int http_server(const int port) {
    const int servfd = socket(AF_INET, SOCK_STREAM, 0);
    if (servfd < 0) {
        return -1;
    }

    const int on = 1;
    // Try to re-use port if already bound
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    // Disable Nagle's algorithm, do not wait for ACK to send
    setsockopt(servfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    // Allow us to decide when to send partial frames by buffering until uncorked
    setsockopt(servfd, IPPROTO_TCP, TCP_CORK, &on, sizeof(on));
    // "Accept" client connection only on first packet, aka first request
    setsockopt(servfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &on, sizeof(on));

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
    // NODELAY should be inherited, but just to make sure here we go
    setsockopt(servfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    // Make sure CORK is on, it's very unclear if this is inherited
    setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &on, sizeof(on));

    return sockfd;
}

void sflush(const int sockfd) {
    // Because we have NODELAY, toggling CORK will force flush any partial
    // frame in the kernel net buffer.
    const int off = 0;
    setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &off, sizeof(off));
    const int on = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &on, sizeof(on));
}

void quickack(const int sockfd) {
    // Force ACK anything pending
    const int on = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &on, sizeof(on));
}

void send_pong(const int sockfd) {
    // Canned empty OK reply
    const char reply[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    send(sockfd, reply, sizeof(reply) - 1, 0);
    sflush(sockfd);
    quickack(sockfd);
}

void send_bad_request(const int sockfd) {
    // Canned Bad Request reply
    const char reply[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    send(sockfd, reply, sizeof(reply) - 1, 0);
    sflush(sockfd);
    quickack(sockfd);
}

int recv_challenge(const int sockfd, cson_t *cson) {
    const size_t BUFFER_SIZE = 2 * 1024 * 1024; // 2MB
    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        return -1;
    }

    int cs = 0;

    %% write init;

    ssize_t n = 0;
    size_t bodylen = 0;
    size_t space = BUFFER_SIZE;
    char *p = buffer, *pe = buffer;
    while (cs < %%{ write first_final; }%% && space > 0 && (n = recv(sockfd, pe, space, 0)) > 0) {
        quickack(sockfd);
        space -= n;
        pe += n;

        %% write exec;
    }
    if (cs < %%{ write first_final; }%% || bodylen + (p - buffer) > BUFFER_SIZE) {
        send_bad_request(sockfd);
        free(buffer);
        return -1;
    }

    cson_init(cson);

    // Process the pre-loaded body chunk
    bodylen -= pe - p;
    p = cson_parse(cson, p, pe);
    while (bodylen > 0) {
        if ((n = recv(sockfd, pe, bodylen, 0)) < 0) {
            send_bad_request(sockfd);
            free(buffer);
            return -1;
        }
        quickack(sockfd);
        bodylen -= n;
        p = cson_parse(cson, p, (pe += n));
    }

    free(buffer);

    return 0;
}

int send_response(const int sockfd, const char *body, const size_t len) {
    char header[512];
    ssize_t n = sprintf(header, "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: application/json\r\nContent-Length: %ld\r\n\r\n", len);
    if (n < 0) {
        return -1;
    }
    if (send(sockfd, header, n, 0) < 0) {
        return -1;
    }
    if (send(sockfd, body, len, 0) < 0) {
        return -1;
    }
    sflush(sockfd);
    quickack(sockfd);
    return 0;
}
