// =============
// THE CHALLENGE
// =============
// We're given a "track", where each number is the distance between stations, e.g.:
// Station #: 0   1   2   3   4   5     (those are implicit)
//  Distance:   2   1   5   3   2   3   (this is what we receive)
//
// We also receive "items", i.e. "queries" for distances between stations on the track:
// (0, 2): Distance between 0 and 2? 2 + 1, so return 3.
// (1, 5): 1 + 5 + 3 + 2 = 11
// We receive multiple queries that we must answer at once.
// Note: the start/end of queries are not necessarily start<end.


// ===========
// THE SCORING
// ===========
// On the infra, the server does a GET first to see that we're up, then sends 75 challenges.
// Examples of challenge sizes observed in practice:
//  - 10    track length,   10       queries
//  - 1500  track length,   1500     queries
//  - 200   track length,   100000   queries
//  - 10000 track length,   10000    queries
// For each of them, the server measures how many seconds it took us to give the right answer.
// Each challenge gets a score of `3 - time_s` (min 0).
// Thus, the theoretical maximum is 75 * 3 = 225 pts.
// So, when we're on the leaderboard at 224.86 pts, it means that we spent 140ms answering challenges.


// ============
// THE SOLUTION
// ============
// The fast solution is easy to find: keep a "running sum" of the sum of all elements from 0 to n, for each n.
// Add an element of '0' at the start to deal with the "implicit" station #0.
// E.g. for the track [1, 3, 2, 4], keep the running_sum [0, 1, 4, 6, 10].
// Then, for each query, we return `running_sum[end] - running_sum[start]` (after making start<end). That's it.


// ==================
// THE REAL CHALLENGE
// ==================
// In practice, this is all done through HTTP & JSON.
// You get in:
// {"items":[[0,1],[2,4]],"track":[13,16,12,3,18,5,2,8,1,14]}
// And you output:
// [13,15]
// From logging the challenges, it looks like the structure of the 75 challenges is:
// ("Nx track T, queries Q" means: N times challenges with track len of T and Q queries)
// 1x   track 10,       queries 10
// 10x  track 100,      queries 100
// 15x  track 1500,     queries 1500
// 15x  track 200,      queries 100000
// 14x  track 10000,    queries 10000
// 1x   track 10,       queries 10
// 10x  track 100,      queries 100
// 9x   track 1500,     queries 1500
//
// After many rounds of iteration, moved _everything_ to C++ and then to C.
// Python is just used to launch our C "HTTP" (or, how I like to call it, CSON) server, which
// really is a super limited server handling a very tiny subset of HTTP/1.1 that reads the socket trying to
// read just the pieces of the challenge.
// Then, it "parses" the JSON with a use-case specific Ragel generated FSM and creats a big list
// of ints for the queries and track, which it can process for the challenge.
// Finally, it writes the final array by writing out a final list of ints.
//
// Our local benchmark tool (on an Intel i7-4600U 2.10GHz) shows, for different sizes of problems:
// track 10,    queries 10:     0.92ms  3.00pts
// track 100,   queries 100:    0.92ms  3.00pts
// track 1500,  queries 1500:   0.98ms  3.00pts
// track 200,   queries 100000: 3.67ms  2.99pts
// track 10000, queries 10000:  1.32ms  3.00pts
//
// This means that speeding up the 15x100k queries challenges is really worth it.


// =====
// NOTES
// =====
// About 80% of the time is spent in the Ragel parser. This is still much
// faster than the hand written one we had previously, but a great place to
// check for possible optimization.
//
// Non-obvious implementation notes:
//  - The `start < end` check is done at parse time due to cache locality of
//    the values at that time.
//  - We rely on the receiving end ignoring whitespace in our JSON output, the
//    `numbers.h` will left-pad digits with a spaces in groups of 4.
//
// Things to check to improve speed:
//  - Using SIMD to parse both in and out of the queries at one.
//  - HTTP header parsing relies on a poor recvline reading char-by-char.
//  - The `numbers.h` serialization speedup is questionable due to likely cache
//    miss.
//
// TODO Add individual part buffers


// ==============
// ARCHIVED NOTES
// ==============
// No longer accurate! Left here for reference.
//
// Notes from local runs (numbers: set 1 / set 2):
//      Name     |    avg time     |           score
// --------------------------------------------------------
// return ""     | 2.11ms / 2.16ms | 224.84pts / 224.84pts   <- if we output nothing
// return input  | 2.73ms / 2.76ms | 224.80pts / 224.79pts   <- if we output the input string (no processing)
// V 17 (fmt)    | 2.79ms / 2.69ms | 224.79pts / 224.80pts   <- our current best version
// Noteworthy: not much wiggle room... our avg time is very close to no processing at all...
//
// From profiling locally, here's the breakdown of our program,
// over 2 sets of 75 challenges:
//  Python benchmark tool says 2.77ms, 244.79pts
//  C++ benchmark says:
//    From socket open to end of last write, avg 0.87ms.
//    Of that 0.87ms, 0.40ms was spent in "processing" & solving the challenge:
//      0.28ms parsing the json string to extract ints
//        (of that, 0.25ms reading 'items', 0.02ms reading 'track' and producing a running_sum)
//      0.01ms solving the challenge (running_sum[end] - running_sum[start])
//      0.10ms writing the final output


#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cson.h"
#include "numbers.h"
#include "server.h"


int solve(int sockfd, cson_t *cson) {
    // Length guess is as follow, 512 extra + (number of queries (aka items / 2) * (8 digits max + comma))
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
        unsigned int dist = track[items[i + 1]] - track[items[i]];
        if (dist > NUMBERS_MAX) {
            unsigned int div = dist / (NUMBERS_MAX + 1), mod = dist % (NUMBERS_MAX + 1);
            *((uint32_t*) p) = NUMBERS[div];
            p += 4;
            // 0x20 is a space ' ', so +0x10 is 0x30 which is zero '0'
            *((uint32_t*) p) = NUMBERS[mod] | 0x10101010;
            p += 4;
        } else {
            *((uint32_t*) p) = NUMBERS[dist];
            p += 4;
        }
        *(p++) = ',';
    }
    buf[0] = '[';
    p[-1] = ']';

    if (send_response(sockfd, buf, p - buf) < 0) {
        free(buf);
        return -1;
    }

    free(buf);
    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

void launch(const int port, const int exit_early) {
    int servfd = http_server(port);
    if (servfd < 0) {
        fprintf(stderr, "Failed to bind to port %d\n", port);
    }
    printf("Listening on port %d\n", port);

    do {
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
            if (solve(sockfd, &cson) < 0) {
                break;
            }
        }

        close(sockfd);
    } while (!exit_early);

    close(servfd);
}

int main(int argc, char **argv) {
    launch(27178, 1);
    return 0;
}

#ifdef __cplusplus
}
#endif
