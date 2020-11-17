#include "cson.h"


%%{
    machine cson;

    action push_digit {
        value = value * 10 + (*p - '0');
    }

    action add_item {
        cson->items[cson->items_size++] = value;
        value = 0;
    }

    action add_track {
        cson->track[cson->track_size++] = cson->track[cson->track_size - 1] + value;
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
    items = ( arr_s ( items_item ( sep items_item )* )? arr_e );

    track_key = "\"track\":";
    track_item = integer %add_track;
    track = ( arr_s ( track_item ( sep track_item )* )? arr_e );

    cson = (
        obj_s
        items_key items sep
        track_key track
        obj_e
    );

    main := cson;
}%%

%% write data;

void cson_init(cson_t *cson) {
    cson->items[0] = 0;
    cson->items_size = 0;
    cson->track[0] = 0;
    cson->track_size = 1;
}

void cson_parse(cson_t *cson, const char *buf, const size_t len) {
    int cs = 0;
    char *p = (char*) buf;
    char *pe = p + len;
    unsigned int value = 0;
    %% write init;
    %% write exec;
}

void cson_free(cson_t *cson) { }
