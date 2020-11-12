#include "cson.h"

#include <stddef.h>
#include <stdlib.h>


void cson_init(cson_t *cson) {
    cson->state = parsing_none;
    cson->items = NULL;
    cson->items_size = 0;
    cson->track = NULL;
    cson->track_size = 0;
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

size_t cson_seek_array_end(cson_t *cson, const char *data, const size_t offset, const size_t len, cson_state_t next_state, int *found) {
    if (found != NULL) {
        *found = 0;
    }
    size_t i = offset;
    for (; i < len; ++i) {
        char c = data[i];
        if (c == ']') {
            cson->state = next_state;
            if (found != NULL) {
                *found = 1;
            }
            ++i;
            break;
        } else if (c != ' ' && c != ',' && c != '\r' && c != '\n' && c != '\t') {
            break;
        }
    }
    return i;
}

size_t cson_parse_uint(const char *data, const size_t offset, const size_t len, unsigned int *value, size_t *parsed) {
    if (parsed != NULL) {
        *parsed = 0;
    }
    size_t i = offset;
    for (; i < len; ++i) {
        char c = data[i];
        if (c != ' ' && c != ',' && c != '\r' && c != '\n' && c != '\t') {
            break;
        }
    }
    unsigned int v = 0;
    for (; i < len; ++i) {
        char c = data[i];
        if (c < '0' || c > '9') {
            break;
        }
        v *= 10;
        v += c - '0';
        if (parsed != NULL) {
            ++(*parsed);
        }
    }
    *value = v;
    return i;
}

void cson_update(cson_t *cson, const char *data, const size_t len) {
    static unsigned int pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

    int flag = 0;
    size_t offset = 0, o = 0, d = 0;
    unsigned int value = 0;
    while (offset < len) {
        switch (cson->state) {
            case parsing_none:
                offset = cson_seek_key(cson, data, offset, len);
                break;
            case parsing_items_pre:
                offset = cson_seek_array_start(cson, data, offset, len, parsing_items, &flag);
                if (flag) {
                    if ((cson->items = malloc(200000 * sizeof(unsigned int))) == NULL) {
                        exit(1);
                    }
                }
                break;
            case parsing_items:
                offset = cson_seek_array_start(cson, data, offset, len, parsing_items_pair, &flag);
                if (!flag && offset < len) {
                    offset = cson_seek_array_end(cson, data, offset, len, parsing_none, NULL);
                }
                break;
            case parsing_items_pair:
                o = cson_parse_uint(data, offset, len, &value, &d);
                if (d > 0) {
                    if (o < len) {
                        if (cson->_partial != 0) {
                            value += cson->_partial * pow10[d];
                            cson->_partial = 0;
                        }
                        cson->items[cson->items_size++] = value;
                    } else if (o >= len) {
                        // Edge of buffer, save in partial and wait for next chunk
                        cson->_partial = value;
                    }
                } else {
                    o = cson_seek_array_end(cson, data, o, len, parsing_items, NULL);
                }
                offset = o;
                break;
            case parsing_track_pre:
                offset = cson_seek_array_start(cson, data, offset, len, parsing_track, &flag);
                if (flag) {
                    if ((cson->track = malloc(10001 * sizeof(unsigned int))) == NULL) {
                        exit(1);
                    }
                }
                break;
            case parsing_track:
                o = cson_parse_uint(data, offset, len, &value, &d);
                if (d > 0) {
                    if (o < len) {
                        if (cson->_partial != 0) {
                            value += cson->_partial * pow10[d];
                            cson->_partial = 0;
                        }
                        cson->track[cson->track_size++] = value;
                    } else if (o >= len) {
                        // Edge of buffer, save in partial and wait for next chunk
                        cson->_partial = value;
                    }
                } else {
                    o = cson_seek_array_end(cson, data, o, len, parsing_none, NULL);
                }
                offset = o;
                break;
        }
    }
}

void cson_free(cson_t *cson) {
    if (cson != NULL) {
        if (cson->items != NULL) {
            free(cson->items);
        }
        if (cson->track != NULL) {
            free(cson->track);
        }
    }
}
