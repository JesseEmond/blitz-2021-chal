#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cson.h"
#include "numbers.h"
#include "server.h"


#ifdef __cplusplus
extern "C" {
#endif

int solve(int sockfd, cson_t *cson) {
    // Guess is as follow, 512 extra + (number of queries (aka items / 2) * (8 digits max + comma))
    size_t guess = 512 + ((cson->items_size / 2) * (8 + 1));
    char* buf = malloc(guess);
    if (buf == NULL) {
        return -1;
    }

    char *p = buf + 1;
    size_t length = cson->items_size;
    unsigned int *items = cson->items;
    unsigned int *track = cson->track;
    for (size_t i = 0; i < length; i += 2) {
        unsigned int dist;
        if (items[i] > items[i + 1]) {
            dist = track[items[i]] - track[items[i + 1]];
        } else {
            dist = track[items[i + 1]] - track[items[i]];
        }
        if (dist > NUMBERS_MAX) {
            // Try to get the compiler to do x86 `div` or something (it generates imul, weird, but whatever)
            unsigned int div = dist / (NUMBERS_MAX + 1), mod = dist % (NUMBERS_MAX + 1);
            *((uint32_t*) p) = NUMBERS[div];
            // 0x20 is a space ' ', so +0x10 is 0x30 which is zero '0'
            *((uint32_t*) (p + 4)) = NUMBERS[mod] | 0x10101010;
            p += 8;
        } else {
            *((uint32_t*) p) = NUMBERS[dist];
            p += 4;
        }
        *(p++) = ',';
    }
    buf[0] = '[';
    p[-1] = ']';

    send_response(sockfd, buf, p - buf);

    free(buf);
    return 0;
}

void launch(const int port, const int exit_early) {
    int servfd = http_server(port);
    if (servfd < 0) {
        fprintf(stderr, "Failed to bind to port %d\n", port);
    }
    printf("Listening on port %d\n", port);

    for (;;) {
        int sockfd = accept_client(servfd);
        if (sockfd < 0) {
            if (errno == EINTR) {
                break;
            } else {
                continue;
            }
        }

        for (;;) {
            cson_t cson;
            if (recv_challenge(sockfd, &cson) < 0) {
                break;
            }
            solve(sockfd, &cson);
            cson_free(&cson);

        }

        close(sockfd);

        if (exit_early) {
            break;
        }
    }

    close(servfd);
}

int main(int argc, char **argv) {
    launch(27178, 1);
    return 0;
}

#ifdef __cplusplus
}
#endif
