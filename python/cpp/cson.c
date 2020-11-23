
#line 1 "cson.rl"
#include "cson.h"


#line 7 "cson.c"
static const int cson_start = 1;


#line 56 "cson.rl"


void cson_init(cson_t *cson) {
    int cs;

    
#line 18 "cson.c"
	{
	cs = cson_start;
	}

#line 62 "cson.rl"

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
    
#line 49 "cson.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 123 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 34 )
		goto st3;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 105 )
		goto st4;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 116 )
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 101 )
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 109 )
		goto st7;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) == 115 )
		goto st8;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) == 34 )
		goto st9;
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) == 58 )
		goto st10;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( (*p) == 91 )
		goto st11;
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) == 91 )
		goto st12;
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr12;
	goto st0;
tr12:
#line 6 "cson.rl"
	{
        value *= 10;
        value += (*p) - '0';
    }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 150 "cson.c"
	if ( (*p) == 44 )
		goto tr13;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr12;
	goto st0;
tr13:
#line 11 "cson.rl"
	{
        item_start = value;
        value = 0;
    }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 167 "cson.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr14;
	goto st0;
tr14:
#line 6 "cson.rl"
	{
        value *= 10;
        value += (*p) - '0';
    }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 182 "cson.c"
	if ( (*p) == 93 )
		goto tr15;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr14;
	goto st0;
tr15:
#line 16 "cson.rl"
	{
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
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 206 "cson.c"
	switch( (*p) ) {
		case 44: goto st11;
		case 93: goto st17;
	}
	goto st0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( (*p) == 44 )
		goto st18;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) == 34 )
		goto st19;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) == 116 )
		goto st20;
	goto st0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) == 114 )
		goto st21;
	goto st0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	if ( (*p) == 97 )
		goto st22;
	goto st0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( (*p) == 99 )
		goto st23;
	goto st0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) == 107 )
		goto st24;
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) == 34 )
		goto st25;
	goto st0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( (*p) == 58 )
		goto st26;
	goto st0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 91 )
		goto st27;
	goto st0;
tr28:
#line 28 "cson.rl"
	{
        track[track_size++] = (track_sum += value);
        value = 0;
    }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 293 "cson.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr27;
	goto st0;
tr27:
#line 6 "cson.rl"
	{
        value *= 10;
        value += (*p) - '0';
    }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 308 "cson.c"
	switch( (*p) ) {
		case 44: goto tr28;
		case 93: goto tr29;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr27;
	goto st0;
tr29:
#line 28 "cson.rl"
	{
        track[track_size++] = (track_sum += value);
        value = 0;
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 327 "cson.c"
	if ( (*p) == 125 )
		goto st30;
	goto st0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 87 "cson.rl"

    cson->_cs = cs;
    cson->_value = value;
    cson->_item_start = item_start;
    cson->_track_sum = track_sum;

    cson->items_size = items_size;
    cson->track_size = track_size;

    return p;
}
