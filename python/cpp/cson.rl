#include "cson.h"


%%{
    machine cson;

    action push_digit {
        value *= 10;
        value += fc - '0';
    }

    action add_item {
        items[items_size] = value;
        ++items_size;
        value = 0;
    }

    action add_track {
        track[track_size] = track[track_size - 1] + value;
        ++track_size;
        value = 0;
    }

    obj_s = "{";
    obj_e = "}";

    arr_s = "[";
    arr_e = "]";

    sep = ",";

    integer = ( digit+ $push_digit );

    items_key = "\"items\":";
    items_item = ( arr_s integer %add_item sep integer %add_item arr_e );
    items = ( arr_s ( items_item ( sep items_item )* ) arr_e );

    track_key = "\"track\":";
    track_item = integer %add_track;
    track = ( arr_s ( track_item ( sep track_item )* ) arr_e );

    cson = ( obj_s items_key items sep track_key track obj_e);

    main := cson;
}%%

%% write data noerror nofinal noentry;

void cson_init(cson_t *cson) {
    int cs = 0;

    %% write init;

    cson->_cs = cs;
    cson->_value = 0;
    cson->items[0] = 0;
    cson->items_size = 0;
    cson->track[0] = 0;
    cson->track_size = 1;
}

size_t cson_update(cson_t *cson, const char *buf, const size_t len) {
    int cs = cson->_cs;
    unsigned int value = cson->_value;
    unsigned int *items = cson->items;
    size_t items_size = cson->items_size;
    unsigned int *track = cson->track;
    size_t track_size = cson->track_size;

    char *p = (char*) buf;
    char *pe = p + len;
    %% write exec;

    cson->_cs = cs;
    cson->_value = value;
    cson->items_size = items_size;
    cson->track_size = track_size;
    return p - buf;
}

void cson_free(cson_t *cson) { }
