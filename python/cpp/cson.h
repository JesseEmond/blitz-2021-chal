#ifndef CSON_H
#define CSON_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum cson_state {
    // Initital state
    parsing_none,
    // Inside "
    parsing_string,
    // Saw 'i', wait for [
    parsing_items_pre,
    // Saw [ after 'i'
    parsing_items,
    // Saw [ after starting [ after 'i'
    parsing_items_pair,
    // Saw 't'
    parsing_track_pre,
    // Saw [ after 't'
    parsing_track,
} cson_state_t;

typedef struct cson {
    cson_state_t state;
    unsigned int *track;
    size_t track_size;
    unsigned int *items;
    size_t items_size;
    unsigned int _partial;
} cson_t;

void cson_init(cson_t *cson);
void cson_update(cson_t *cson, const char *data, const size_t len);
void cson_free(cson_t *cson);

#ifdef __cplusplus
}
#endif

#endif
