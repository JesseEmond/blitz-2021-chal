#ifndef PARSING_H
#define PARSING_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t fast_atoi(const char* str, const size_t len, unsigned int *value);

#ifdef __cplusplus
}
#endif

#endif /* PARSING_H */
