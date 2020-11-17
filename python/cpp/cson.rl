#include "cson.h"

#include "parsing.h"

%%{
    machine cson;

    action push_digit {
        digit_buffer[digit_pos++] = *p;
    }

    action parse_int {
        fast_atoi(digit_buffer, digit_pos, &value);
        digit_pos = 0;
    }

    action add_item {
        cson->items[cson->items_size++] = value;
    }

    action add_track {
        cson->track[cson->track_size++] = cson->track[cson->track_size - 1] + value;
    }

    obj_s = "{";
    obj_e = "}";

    arr_s = "[";
    arr_e = "]";

    sep = ",";

    integer = ( digit+ $push_digit %parse_int );

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
    char *p = (char*) buf, *pe = (char*) buf + len;
    char digit_buffer[16];
    size_t digit_pos = 0;
    unsigned int value;
    %% write init;
    %% write exec;
}

void cson_free(cson_t *cson) { }
