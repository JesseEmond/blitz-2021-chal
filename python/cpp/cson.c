
#line 1 "cson.rl"
#include "cson.h"



#line 44 "cson.rl"



#line 12 "cson.c"
static const int cson_start = 1;


#line 47 "cson.rl"

void cson_init(cson_t *cson) {
    int cs = 0;

    
#line 22 "cson.c"
	{
	cs = cson_start;
	}

#line 52 "cson.rl"

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
    
#line 48 "cson.c"
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
	switch( (*p) ) {
		case 91: goto st12;
		case 93: goto st18;
	}
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr13;
	goto st0;
tr13:
#line 7 "cson.rl"
	{
        value = value * 10 + ((*p) - '0');
    }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 150 "cson.c"
	if ( (*p) == 44 )
		goto tr14;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr13;
	goto st0;
tr14:
#line 11 "cson.rl"
	{
        items[items_size] = value;
        ++items_size;
        value = 0;
    }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 168 "cson.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr15;
	goto st0;
tr15:
#line 7 "cson.rl"
	{
        value = value * 10 + ((*p) - '0');
    }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 182 "cson.c"
	if ( (*p) == 93 )
		goto tr16;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr15;
	goto st0;
tr16:
#line 11 "cson.rl"
	{
        items[items_size] = value;
        ++items_size;
        value = 0;
    }
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
#line 200 "cson.c"
	switch( (*p) ) {
		case 44: goto st17;
		case 93: goto st18;
	}
	goto st0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( (*p) == 91 )
		goto st12;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) == 44 )
		goto st19;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) == 34 )
		goto st20;
	goto st0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) == 116 )
		goto st21;
	goto st0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	if ( (*p) == 114 )
		goto st22;
	goto st0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( (*p) == 97 )
		goto st23;
	goto st0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) == 99 )
		goto st24;
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) == 107 )
		goto st25;
	goto st0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( (*p) == 34 )
		goto st26;
	goto st0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 58 )
		goto st27;
	goto st0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) == 91 )
		goto st28;
	goto st0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( (*p) == 93 )
		goto st31;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr28;
	goto st0;
tr28:
#line 7 "cson.rl"
	{
        value = value * 10 + ((*p) - '0');
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 302 "cson.c"
	switch( (*p) ) {
		case 44: goto tr30;
		case 93: goto tr31;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr28;
	goto st0;
tr30:
#line 17 "cson.rl"
	{
        track[track_size] = track[track_size - 1] + value;
        ++track_size;
        value = 0;
    }
	goto st30;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
#line 322 "cson.c"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr28;
	goto st0;
tr31:
#line 17 "cson.rl"
	{
        track[track_size] = track[track_size - 1] + value;
        ++track_size;
        value = 0;
    }
	goto st31;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
#line 338 "cson.c"
	if ( (*p) == 125 )
		goto st32;
	goto st0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
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
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 72 "cson.rl"

    cson->_cs = cs;
    cson->_value = value;
    cson->items_size = items_size;
    cson->track_size = track_size;
    return p - buf;
}

void cson_free(cson_t *cson) { }
