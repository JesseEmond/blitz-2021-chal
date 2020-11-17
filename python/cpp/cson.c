#include "cson.h"

#include <stddef.h>
#include <stdlib.h>

#include "expect.h"
#include "parsing.h"
#include "prof.h"


void cson_init(cson_t *cson) {
    cson->state = parsing_none;
    cson->items[0] = 0;
    cson->items_size = 0;
    cson->track[0] = 0;
    cson->track_size = 1;
    cson->_partial = 0;
}

size_t cson_seek_key(cson_t *cson, const char *data, const size_t offset, const size_t len) {
    size_t i = offset;
    for (; i < len; ++i) {
        char c = data[i];
        if (c == 'i') {
            cson->state = parsing_items_pre;
            return i + 5; // Skip the string content immediately
        } else if (c == 't') {
            cson->state = parsing_track_pre;
            return i + 5; // Skip the string content immediately
        }
    }
    return i;
}

size_t cson_seek_array_start(cson_t *cson, const char *data, const size_t offset, const size_t len, cson_state_t next_state, int *found) {
    if (found != NULL) {
        *found = 0;
    }
    size_t i = offset;
    for (; i < len; ++i) {
        char c = data[i];
        if (c == '[') {
            cson->state = next_state;
            if (found != NULL) {
                *found = 1;
            }
            return i + 1;
        } else if (c == ']') {
            break;
        }
    }
    return i;
}

size_t cson_seek_array_end(cson_t *cson, const char *data, const size_t offset, const size_t len, cson_state_t next_state) {
    size_t i = offset;
    for (; i < len; ++i) {
        char c = data[i];
        if (c == ']') {
            cson->state = next_state;
            return i + 1;
        } else if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
            break;
        }
    }
    return i;
}

// FIXME This is 50% of the time apparently
size_t cson_parse_uint(cson_t *cson, const char *data, const size_t offset, const size_t len, unsigned int *value, int *found) {
    static unsigned int pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
    //PROF_START();

    unsigned int v = cson->_partial;

    // Skip whitespace stuff and ',' only if we don't have a partial run
    size_t o = offset;
    for (; o < len; ++o) {
        char c = data[o];
        if (c == ',') {
            if (unlikely(v != 0)) {
                break;
            }
        } else if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
            break;
        }
    }

    unsigned int x = 0;
    size_t l = fast_atoi(data + o, len - o, &x);
    *value = v = v * pow10[l] + x;
    if (unlikely(o + l == len)) {
        // On input edge, save partial run for later
        cson->_partial = v;
        if (unlikely(l > 0 && v == 0)) {
            // Special 0 case on edge
            *found = 1;
        } else {
            *found = 0;
        }
    } else if (likely(l > 0 || v != 0)) {
        // On digits edge or leftover partial run, return found
        cson->_partial = 0;
        *found = 1;
    } else {
        // No parse and no leftover
        *found = 0;
    }
    // From start of function to here is avg 0.0006ms (or 6ns)
    //PROF_END("cson_parse_uint", 100000);
    return o + l;
}

void cson_update(cson_t *cson, const char *data, const size_t len) {
    int found = 0;
    size_t offset = 0;
    unsigned int value = 0;
    while (offset < len) {
        switch (cson->state) {
            case parsing_none:
                offset = cson_seek_key(cson, data, offset, len);
                break;
            case parsing_items_pre:
                offset = cson_seek_array_start(cson, data, offset, len, parsing_items, NULL);
                break;
            case parsing_items:
                offset = cson_seek_array_start(cson, data, offset, len, parsing_items_pair, &found);
                if (!found && offset < len) {
                    offset = cson_seek_array_end(cson, data, offset, len, parsing_none);
                }
                break;
            case parsing_items_pair:
                while (offset < len) {
                    offset = cson_parse_uint(cson, data, offset, len, &value, &found);
                    if (found) {
                        cson->items[cson->items_size++] = value;
                    } else {
                        offset = cson_seek_array_end(cson, data, offset, len, parsing_items);
                        break;
                    }
                }
                break;
            case parsing_track_pre:
                offset = cson_seek_array_start(cson, data, offset, len, parsing_track, NULL);
                break;
            case parsing_track:
                while (offset < len) {
                    offset = cson_parse_uint(cson, data, offset, len, &value, &found);
                    if (found) {
                        // Running sum value
                        cson->track[cson->track_size++] = cson->track[cson->track_size - 1] + value;
                    } else {
                        offset = cson_seek_array_end(cson, data, offset, len, parsing_none);
                        break;
                    }
                }
                break;
        }
    }
}

void cson_free(cson_t *cson) { }
