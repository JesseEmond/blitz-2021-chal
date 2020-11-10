#ifndef MEASURE_H
#define MEASURE_H

#include <iomanip>
#include <iostream>
#include <string>

// If PROFILE_OUTPUT_N is defined, outputs avg ms stats every
// PROFILE_OUTPUT_N calls.
template <class F>
void measure(const std::string name, F f) {
    #ifdef PROFILE_OUTPUT_N
        static double total = 0.0;
        static int count = 0;
        const clock_t start = clock();
    #endif

    f();

    #ifdef PROFILE_OUTPUT_N
        const clock_t end = clock();
        total += (double)(end - start) / CLOCKS_PER_SEC;
        ++count;
        if (count % PROFILE_OUTPUT_N == 0) {
            std::cout << "<avg time for " << name << " ("
                      << count << " calls): "
                      << std::fixed << std::setprecision(2)
                      << total / count * 1000.0 << "ms>"
                      << '\n';
        }
    #endif
}

#endif