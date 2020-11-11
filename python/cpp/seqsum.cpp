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
// After many rounds of iteration, moved _everything_ to C++.
// Python is just used to launch our C++ "HTTP" server, which
// really is a supra-hardcoded program that reads the socket trying to
// read just the pieces of the challenge.
// Then, it "parses" the JSON by skipping everything and creating a big list
// of ints, which it can process for the challenge.
// Finally, it writes the final array by writing out a final list of ints.
//
// Our local benchmark tool shows, for different sizes of problems:
// track 10,    queries 10:     1.75ms  3.00pts
// track 100,   queries 100:    1.78ms  3.00pts
// track 1500,  queries 1500:   1.83ms  3.00pts
// track 200,   queries 100000: 5.77ms  2.99pts
// track 10000, queries 10000:  2.29ms  3.00pts
//
// This means that speeding up the 15x100k queries challenges is really worth it.


// =====
// NOTES
// =====
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



// Enable this #define to get profiling numbers from the C++ program itself, where measured.
// Show profile timers every N calls.
#define PROFILE_OUTPUT_N 500


#include "challenge.h"
#include "measure.h"
#include "server.h"

#include <iostream>
#include <unistd.h>
#include <string_view>


extern "C" {
    // Launches our "HTTP" server, waiting to solve challenges.
    void launch(int port) {
        int servfd = http_server(port);
        if (servfd < 0) {
            std::cerr << "Failed to bind to port " << port << std::endl;
            exit(1);
        }
        std::cout << "Listening on port " << port << std::endl;

        while (true) {
            int sockfd = accept_client(servfd);
            if (sockfd < 0) {
                if (errno == EINTR) {
                    break;
                } else {
                    continue;
                }
            }

            // TODO: Can we just fork() here? Are requests sent in parallel?

            bool keep_alive = true;
            while (keep_alive) {
                measure("end2end", [&] {
                    ssize_t len = 0;
                    char *buf = NULL;
                    if ((len = recv_challenge(sockfd, &buf)) > 0) {
                        std::string_view chal(buf, static_cast<std::string_view::size_type>(len));
                        std::string_view sln;
                        measure("end2end::solve", [&] {
                            sln = seqsum(chal);
                        });
                        send_response(sockfd, sln.data(), sln.size());
                        free(buf);
                    } else {
                        keep_alive = false;
                    }
                });
            }

            close(sockfd);
        }
    }
}
