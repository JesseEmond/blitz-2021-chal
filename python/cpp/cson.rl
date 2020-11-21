#include "cson.h"


%%{
    machine cson;

    action push_digit {
        value *= 10;
        value += fc - '0';
    }

    action item_start {
        item_start = value;
        value = 0;
    }

    action item_end {
        // The start < end check is done here because of cache locality
        if (item_start > value) {
            items[items_size++] = value;
            items[items_size++] = item_start;
        } else {
            items[items_size++] = item_start;
            items[items_size++] = value;
        }
        value = 0;
    }

    action add_track {
        track[track_size++] = (track_sum += value);
        value = 0;
    }

    obj_s = "{";
    obj_e = "}";

    arr_s = "[";
    arr_e = "]";

    sep = ",";

    integer = digit+ $push_digit;

    items_key = "\"items\":";
    items_item = arr_s integer %item_start sep integer %item_end arr_e;
    items = arr_s items_item ( sep items_item )* arr_e;

    track_key = "\"track\":";
    track_item = integer %add_track;
    track = arr_s track_item ( sep track_item )* arr_e;

    cson = obj_s items_key items sep track_key track obj_e;

    main := cson;
}%%

%% write data noerror nofinal noentry;

void cson_init(cson_t *cson) {
    int cs;

    %% write init;

    cson->_cs = cs;
    cson->_value = 0;
    cson->_item_start = 0;
    cson->_track_sum = 0;

    cson->items_size = 0;
    cson->track[0] = 0;
    cson->track_size = 1;
}

char *cson_parse(cson_t *cson, const char *start, const char *end) {
    int cs = cson->_cs;
    unsigned int value = cson->_value;
    unsigned int item_start = cson->_item_start;
    unsigned int track_sum = cson->_track_sum;

    unsigned int *items = cson->items;
    size_t items_size = cson->items_size;
    unsigned int *track = cson->track;
    size_t track_size = cson->track_size;

    char *p = (char*) start;
    char *pe = (char*) end;;
    %% write exec;

    cson->_cs = cs;
    cson->_value = value;
    cson->_item_start = item_start;
    cson->_track_sum = track_sum;

    cson->items_size = items_size;
    cson->track_size = track_size;

    return p;
}
