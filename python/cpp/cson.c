#include "cson.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


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
            i += 5; // Skip the string content immediately
            break;
        } else if (c == 't') {
            cson->state = parsing_track_pre;
            i += 5; // Skip the string content immediately
            break;
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
            ++i;
            break;
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
        if (c == ' ' || c == '\r' || c == '\n' || c == '\t') {
            continue;
        } else if (c == ']') {
            cson->state = next_state;
            ++i;
            break;
        }
        break;
    }
    return i;
}

// FIXME This is 50% of the time apparently
size_t cson_parse_uint(cson_t *cson, const char *data, const size_t offset, const size_t len, unsigned int *value, int *found) {
    static unsigned int pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

    unsigned int v = cson->_partial;

    // Skip whitespace stuff and ',' only if we don't have a partial run
    size_t o = offset;
    for (; o < len; ++o) {
        char c = data[o];
        if (c == ' ' || (c == ',' && v == 0) || c == '\r' || c == '\n' || c == '\t') {
            continue;
        }
        break;
    }

    size_t i = o;
    // TODO Unroll this
    for (; i < len; ++i) {
        char c = data[i];
        if (c < '0' || c > '9') {
            break;
        }
    }

    size_t l = i - o;
    if (l > 0) {
        if (l > sizeof(uint64_t)) {
            // Unhandled lol
            exit(1);
        }
        // Adapted weird dark magic from
        // https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html
        uint64_t x = 0;
        memcpy(((void*)&x) + (sizeof(uint64_t) - l), data + o, l);
        x = ((x & 0x0f000f000f000f00) >> 8) + ((x & 0x000f000f000f000f) * 10);
        x = ((x & 0x00ff000000ff0000) >> 16) + ((x & 0x000000ff000000ff) * 100);
        x = ((x & 0x0000ffff00000000) >> 32) + ((x & 0x000000000000ffff) * 10000);
        v = v != 0 ? v * pow10[l] + (unsigned int)x : (unsigned int)x;
    }

    if (i == len) {
        // On input edge, save partial run for later
        cson->_partial = v;
        if (l > 0 && v == 0) {
            // Special 0 case on edge
            *found = 1;
            *value = 0;
        } else {
            *found = 0;
        }
        return i;
    } else if (l > 0 || v != 0) {
        // On digits edge or leftover partial run, return found
        cson->_partial = 0;
        *found = 1;
        *value = v;
    } else {
        // No parse and no leftover
        *found = 0;
    }
    return i;
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
                offset = cson_parse_uint(cson, data, offset, len, &value, &found);
                if (found) {
                    cson->items[cson->items_size++] = value;
                } else {
                    offset = cson_seek_array_end(cson, data, offset, len, parsing_items);
                }
                break;
            case parsing_track_pre:
                offset = cson_seek_array_start(cson, data, offset, len, parsing_track, NULL);
                break;
            case parsing_track:
                offset = cson_parse_uint(cson, data, offset, len, &value, &found);
                if (found) {
                    // Running sum value
                    cson->track[cson->track_size++] = cson->track[cson->track_size - 1] + value;
                } else {
                    offset = cson_seek_array_end(cson, data, offset, len, parsing_none);
                }
                break;
        }
    }
}

void cson_free(cson_t *cson) { }
