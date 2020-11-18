#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cson.h"
#include "expect.h"
#include "numbers.h"
#include "prof.h"
#include "server.h"
#include "string.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct solve_data {
    cson_t *cson;
    size_t offset;
    size_t length;
    char *output;
    char *output_end;
} solve_data_t;

void *solve_thread(void *arg) {
    solve_data_t *data = (solve_data_t*) arg;

    unsigned int *track = data->cson->track;
    unsigned int *items = data->cson->items;
    size_t length = data->length;
    char* p = data->output;

    for (size_t i = data->offset; likely(i < length); i += 2) {
        size_t s = items[i];
        size_t e = items[i + 1];
        if (unlikely(s > e)) {
            s = s ^ e;
            e = e ^ s;
            s = s ^ e;
        }
        unsigned int dist = track[e] - track[s];
        if (likely(dist <= NUMBERS_MAX)) {
            const char *digits = NUMBERS[dist];
            p[0] = digits[0];
            p[1] = digits[1];
            p[2] = digits[2];
            p[3] = digits[3];
            p += 4;
        } else {
            p += sprintf(p, "%d", dist);
        }
        *(p++) = ',';
    }

    data->output_end = p;

    return NULL;
}

int solve(int sockfd, cson_t *cson) {
    PROF_START(solve);
    // Guess is as follow, 512 extra + (number of queries (aka items / 2) * (6 digits max + comma))
    size_t len_guess = 512 + ((cson->items_size / 2) * (6 + 1));
    char* buf = malloc(len_guess);
    if (buf == NULL) {
        return -1;
    }

    // Thread handling is more expensive than speedup for small challenges
    if (likely(cson->items_size > 200)) {
        send_response_headers(sockfd);

        // Blank first half with whitespace
        memset(buf, ' ', len_guess / 2);

        solve_data_t tdata[2];

        tdata[0].cson = cson;
        tdata[0].offset = 0;
        tdata[0].length = (cson->items_size / 2);
        tdata[0].output = buf + 1;

        tdata[1].cson = cson;
        tdata[1].offset = (cson->items_size / 2);
        tdata[1].length = cson->items_size;
        tdata[1].output = buf + (len_guess / 2);

        pthread_t tids[2];
        pthread_create(&tids[0], NULL, solve_thread, (void*) &tdata[0]);
        pthread_create(&tids[1], NULL, solve_thread, (void*) &tdata[1]);

        pthread_join(tids[0], NULL);
        buf[0] = '[';
        send_response_chunk(sockfd, buf, tdata[0].output_end - buf);

        pthread_join(tids[1], NULL);
        tdata[1].output_end[-1] = ']';
        send_response_chunk(sockfd, tdata[1].output, tdata[1].output_end - tdata[1].output);

        send_response_chunk(sockfd, NULL, 0);
    } else {
        solve_data_t data;
        data.cson = cson;
        data.offset = 0;
        data.length = cson->items_size;
        data.output = buf + 1;

        solve_thread((void*) &data);

        buf[0] = '[';
        data.output_end[-1] = ']';
        send_response(sockfd, buf, data.output_end - buf);
    }

    free(buf);
    PROF_END(solve, 500);
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
