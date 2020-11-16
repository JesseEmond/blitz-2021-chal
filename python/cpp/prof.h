#ifndef PROF_H
#define PROF_H

#include <time.h>
#include <stdio.h>

#define PROF_START() \
    static int prof_count__ = 0; \
    static double prof_total__ = 0.0; \
    const clock_t prof_start__ = clock();

#define PROF_END(n, c) \
    const clock_t prof_end__ = clock(); \
    const double prof_delta__ = (double) (prof_end__ - prof_start__) / CLOCKS_PER_SEC; \
    prof_total__ += prof_delta__; \
    if (++prof_count__ % c == 0) { \
        printf("%s (%d calls) - avg: %.4fms last: %.4fms total: %.2fms\n", n, prof_count__, prof_total__ / prof_count__ * 1000.0, prof_delta__ * 1000.0, prof_total__ * 1000.0); \
    }

#endif /* PROF_H */
