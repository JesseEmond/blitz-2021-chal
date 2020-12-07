#ifndef CSON_H
#define CSON_H

#include <sys/types.h>


#define ITEMS_MAX 100000
#define TRACK_MAX 10000

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cson {
    int _cs;
    int _value;
    int _track_sum;

    int items[ITEMS_MAX * 2];
    size_t items_size;
    int track[TRACK_MAX + 1];
    size_t track_size;
} cson_t;

void cson_init(cson_t *cson);
char *cson_parse(cson_t *cson, const char *start, const char *end);

#ifdef __cplusplus
}
#endif

#endif
