#include "cson.h"

#include <stddef.h>
#include <stdlib.h>


void cson_init(cson_t *cson) {
    cson->state = parsing_none;
    cson->track = NULL;
    cson->track_size = 0;
    cson->items = NULL;
    cson->items_size = 0;
}

void cson_update(cson_t *cson, const char *data, const size_t len) {
    switch (cson->state) {
        case parsing_none:
            break;
        case parsing_string:
            break;
        case parsing_items_pre:
            break;
        case parsing_items:
            break;
        case parsing_items_pair:
            break;
        case parsing_track_pre:
            break;
        case parsing_track:
            break;
    }
}

void cson_free(cson_t *cson) {
    if (cson->track != NULL) {
        free(cson->track);
    }
    if (cson->items != NULL) {
        free(cson->items);
    }
}
