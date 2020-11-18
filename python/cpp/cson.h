#ifndef CSON_H
#define CSON_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cson {
    int _cs;
    unsigned int _value;

    unsigned int items[100000 * 2];
    size_t items_size;
    unsigned int track[10000 + 1];
    size_t track_size;
} cson_t;

void cson_init(cson_t *cson);
size_t cson_update(cson_t *cson, const char *buf, const size_t len);
void cson_free(cson_t *cson);

#ifdef __cplusplus
}
#endif

#endif
