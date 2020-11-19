#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cson.h"
#include "expect.h"
#include "numbers.h"
#include "server.h"


#ifdef __cplusplus
extern "C" {
#endif

char *solve_part(char *p, unsigned int *track, unsigned int *items, const size_t offset, const size_t length) {
    for (size_t i = offset; likely(i < length); i += 2) {
        size_t s = items[i];
        size_t e = items[i + 1];
        if (unlikely(s > e)) {
            s ^= e;
            e ^= s;
            s ^= e;
        }
        unsigned int dist = track[e] - track[s];
        if (unlikely(dist > NUMBERS_MAX)) {
            *((uint32_t*) p) = *((uint32_t*) NUMBERS[dist / (NUMBERS_MAX + 1)]);
            // 0x20 is a space, so +0x10 is 0x30 which is '0'
            *((uint32_t*) (p + 4)) = (*((uint32_t*) NUMBERS[dist % (NUMBERS_MAX + 1)])) | 0x10101010;
            p += 8;
        } else {
            *((uint32_t*) p) = *((uint32_t*) NUMBERS[dist]);
            p += 4;
        }
        *(p++) = ',';
    }
    return p;
}

typedef struct solve_data {
    cson_t *cson;
    size_t offset;
    size_t length;
    char *output;
    char *output_end;
} solve_data_t;

void *solve_thread(void *arg) {
    solve_data_t *data = (solve_data_t*) arg;
    data->output_end = solve_part(data->output, data->cson->track, data->cson->items,
                                  data->offset, data->length);
    return NULL;
}

int solve(int sockfd, cson_t *cson) {
    // Guess is as follow, 512 extra + (number of queries (aka items / 2) * (8 digits max + comma))
    size_t guess = 512 + ((cson->items_size / 2) * (8 + 1));
    char* buf = malloc(guess);
    if (buf == NULL) {
        return -1;
    }

    // Thread handling is more expensive than speedup for small challenges
    if (likely(cson->items_size > 200)) {
        solve_data_t tdata[2];

        tdata[0].cson = cson;
        tdata[0].offset = 0;
        tdata[0].length = (cson->items_size / 2);
        tdata[0].output = buf + 1;

        tdata[1].cson = cson;
        tdata[1].offset = (cson->items_size / 2);
        tdata[1].length = cson->items_size;
        tdata[1].output = buf + (guess / 2);

        pthread_t tids[2];
        pthread_create(&tids[0], NULL, solve_thread, (void*) &tdata[0]);
        pthread_create(&tids[1], NULL, solve_thread, (void*) &tdata[1]);

        send_response_headers(sockfd);

        pthread_join(tids[0], NULL);
        buf[0] = '[';
        send_response_chunk(sockfd, buf, tdata[0].output_end - buf);

        pthread_join(tids[1], NULL);
        tdata[1].output_end[-1] = ']';
        send_response_chunk(sockfd, tdata[1].output, tdata[1].output_end - tdata[1].output);

        send_response_chunk(sockfd, NULL, 0);
    } else {
        char *end = solve_part(buf + 1, cson->track, cson->items, 0, cson->items_size);

        buf[0] = '[';
        end[-1] = ']';
        send_response(sockfd, buf, end - buf);
    }

    free(buf);
    return 0;
}

void launch(const int port) {
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
    }

    close(servfd);
}

int main(int argc, char **argv) {
    launch(27178);
    return 0;
}

#ifdef __cplusplus
}
#endif
