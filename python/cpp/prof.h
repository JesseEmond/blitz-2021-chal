#ifndef PROF_H
#define PROF_H

#include <time.h>
#include <stdio.h>

#include "config.h"


#if PROF_ENABLE

#define PROF_START(n) \
    static int _prof_count__##n = 0; \
    static double _prof_total__##n = 0.0; \
    const clock_t _prof_start__##n = clock();

#define PROF_END(n, s) \
    const clock_t _prof_end__##n = clock(); \
    const double _prof_delta__##n = (double) (_prof_end__##n - _prof_start__##n) / CLOCKS_PER_SEC; \
    _prof_total__##n += _prof_delta__##n; \
    if (++_prof_count__##n % s == 0) { \
        printf(#n" (%d calls) - avg: %.4fms last: %.4fms total: %.2fms\n", _prof_count__##n, _prof_total__##n / _prof_count__##n * 1000.0, _prof_delta__##n * 1000.0, _prof_total__##n * 1000.0); \
    }

#else

#define PROF_START(n)

#define PROF_END(n, c)

#endif

#endif /* PROF_H */
