#include "parsing.h"

#include <stdint.h>

#include "expect.h"


size_t fast_atoi(const char *str, const size_t len, unsigned int *value) {
    //PROF_START();

    // Adapted weird dark magic from
    // https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html
    // TODO Check for SIMD darker magic
    // http://0x80.pl/articles/simd-parsing-int-sequences.html#precalculating-str

    size_t i = 0;
    uint64_t x = 0;
    // Figure out digits length
    for (; i < len; ++i) {
        char c = str[i];
        if (likely(c >= '0' && c <= '9')) {
            x = (x >> 8) | (((uint64_t) c) << 56);
        } else {
            break;
        }
    }

    if (likely(i > 1)) {
        x = ((x & 0x0f000f000f000f00) >> 8) + ((x & 0x000f000f000f000f) * 10);
        x = ((x & 0x00ff000000ff0000) >> 16) + ((x & 0x000000ff000000ff) * 100);
        x = ((x & 0x0000ffff00000000) >> 32) + ((x & 0x000000000000ffff) * 10000);
        *value = (unsigned int) x;
    } else if (i == 1) {
        *value = (x >> 56) & 0x0f;
    } else {
        *value = 0;
    }

    // From start of function to here avg is 0.0006ms (or 6ns)
    //PROF_END("atoi", 1000000);
    return i;
}
