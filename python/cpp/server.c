
#line 1 "server.rl"
#include "server.h"

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cson.h"


#line 16 "server.c"
static const int http_request_start = 1;


#line 44 "server.rl"



int http_server(const int port) {
    const int servfd = socket(AF_INET, SOCK_STREAM, 0);
    if (servfd < 0) {
        return -1;
    }

    const int on = 1;
    // Try to re-use port if already bound
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    // Disable Nagle's algorithm, do not wait for ACK to send
    setsockopt(servfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    // Allow us to decide when to send partial frames by buffering until uncorked
    setsockopt(servfd, IPPROTO_TCP, TCP_CORK, &on, sizeof(on));
    // "Accept" client connection only on first packet, aka first request
    setsockopt(servfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &on, sizeof(on));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(servfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(servfd);
        return -1;
    }

    if (listen(servfd, 10) < 0) {
        close(servfd);
        return -1;
    }

    return servfd;
}

int accept_client(const int servfd) {
    const int sockfd = accept(servfd, NULL, NULL);
    if (sockfd < 0) {
        return -1;
    }

    const int on = 1;
    // NODELAY should be inherited, but just to make sure here we go
    setsockopt(servfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
    // Make sure CORK is on, it's very unclear if this is inherited
    setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &on, sizeof(on));

    return sockfd;
}

void sflush(const int sockfd) {
    // Because we have NODELAY, toggling CORK will force flush any partial
    // frame in the kernel net buffer.
    const int off = 0;
    setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &off, sizeof(off));
    const int on = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &on, sizeof(on));
}

void quickack(const int sockfd) {
    const int on = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &on, sizeof(on));
}

void send_pong(const int sockfd) {
    // Canned empty OK reply
    const char reply[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    send(sockfd, reply, sizeof(reply) - 1, 0);
    sflush(sockfd);
    quickack(sockfd);
}

void send_bad_request(const int sockfd) {
    // Canned Bad Request reply
    const char reply[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    send(sockfd, reply, sizeof(reply) - 1, 0);
    sflush(sockfd);
    quickack(sockfd);
}

int recv_challenge(const int sockfd, cson_t *cson) {
    const size_t MAX_SIZE = 2 * 1024 * 1024; // 2MB
    char *buffer = malloc(MAX_SIZE);
    if (buffer == NULL) {
        return -1;
    }

    int cs = 0;

    
#line 114 "server.c"
	{
	cs = http_request_start;
	}

#line 137 "server.rl"

    ssize_t n = 0;
    size_t bodylen = 0;
    size_t space = MAX_SIZE;
    char *p = buffer, *pe = buffer;
    while (cs < 34 && space > 0 && (n = recv(sockfd, pe, space, 0)) > 0) {
        quickack(sockfd);
        space -= n;
        pe += n;

        
#line 131 "server.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	switch( (*p) ) {
		case 71: goto st2;
		case 80: goto st30;
	}
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 69 )
		goto st3;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 84 )
		goto st4;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 32 )
		goto tr5;
	goto st0;
tr5:
#line 15 "server.rl"
	{
        send_pong(sockfd);
        free(buffer);
        return -1;
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 179 "server.c"
	if ( 32 <= (*p) && (*p) <= 126 )
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 13 )
		goto st7;
	if ( 32 <= (*p) && (*p) <= 126 )
		goto st6;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) == 10 )
		goto st8;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 67: goto st14;
		case 99: goto st14;
	}
	if ( (*p) > 57 ) {
		if ( 59 <= (*p) && (*p) <= 126 )
			goto st9;
	} else if ( (*p) >= 33 )
		goto st9;
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) == 58 )
		goto st10;
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( (*p) == 13 )
		goto st11;
	if ( 32 <= (*p) && (*p) <= 126 )
		goto st10;
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) == 10 )
		goto st12;
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 13: goto st13;
		case 67: goto st14;
		case 99: goto st14;
	}
	if ( (*p) > 57 ) {
		if ( 59 <= (*p) && (*p) <= 126 )
			goto st9;
	} else if ( (*p) >= 33 )
		goto st9;
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( (*p) == 10 )
		goto tr15;
	goto st0;
tr15:
#line 41 "server.rl"
	{ {p++; cs = 34; goto _out;} }
	goto st34;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
#line 268 "server.c"
	goto st0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	switch( (*p) ) {
		case 58: goto st10;
		case 79: goto st15;
		case 111: goto st15;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	switch( (*p) ) {
		case 58: goto st10;
		case 78: goto st16;
		case 110: goto st16;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	switch( (*p) ) {
		case 58: goto st10;
		case 84: goto st17;
		case 116: goto st17;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	switch( (*p) ) {
		case 58: goto st10;
		case 69: goto st18;
		case 101: goto st18;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	switch( (*p) ) {
		case 58: goto st10;
		case 78: goto st19;
		case 110: goto st19;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	switch( (*p) ) {
		case 58: goto st10;
		case 84: goto st20;
		case 116: goto st20;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	switch( (*p) ) {
		case 45: goto st21;
		case 58: goto st10;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	switch( (*p) ) {
		case 58: goto st10;
		case 76: goto st22;
		case 108: goto st22;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	switch( (*p) ) {
		case 58: goto st10;
		case 69: goto st23;
		case 101: goto st23;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	switch( (*p) ) {
		case 58: goto st10;
		case 78: goto st24;
		case 110: goto st24;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	switch( (*p) ) {
		case 58: goto st10;
		case 71: goto st25;
		case 103: goto st25;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	switch( (*p) ) {
		case 58: goto st10;
		case 84: goto st26;
		case 116: goto st26;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	switch( (*p) ) {
		case 58: goto st10;
		case 72: goto st27;
		case 104: goto st27;
	}
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) == 58 )
		goto st28;
	if ( 33 <= (*p) && (*p) <= 126 )
		goto st9;
	goto st0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	switch( (*p) ) {
		case 13: goto st11;
		case 32: goto st28;
	}
	if ( (*p) < 48 ) {
		if ( 33 <= (*p) && (*p) <= 47 )
			goto st10;
	} else if ( (*p) > 57 ) {
		if ( 58 <= (*p) && (*p) <= 126 )
			goto st10;
	} else
		goto tr30;
	goto st0;
tr30:
#line 21 "server.rl"
	{
        bodylen *= 10;
        bodylen += (*p) - '0';
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 462 "server.c"
	if ( (*p) == 13 )
		goto st11;
	if ( (*p) < 48 ) {
		if ( 32 <= (*p) && (*p) <= 47 )
			goto st10;
	} else if ( (*p) > 57 ) {
		if ( 58 <= (*p) && (*p) <= 126 )
			goto st10;
	} else
		goto tr30;
	goto st0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) == 79 )
		goto st31;
	goto st0;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	if ( (*p) == 83 )
		goto st32;
	goto st0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	if ( (*p) == 84 )
		goto st33;
	goto st0;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	if ( (*p) == 32 )
		goto st5;
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
	_test_eof34: cs = 34; goto _test_eof; 
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
	_test_eof33: cs = 33; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 148 "server.rl"
    }

    if (cs < 34 || bodylen + (p - buffer) > MAX_SIZE) {
        send_bad_request(sockfd);
        free(buffer);
        return -1;
    }

    cson_init(cson);

    if (p != pe) {
        // Process the pre-loaded body chunk
        bodylen -= pe - p;
        p = cson_parse(cson, p, pe);
    }
    while (bodylen > 0) {
        if ((n = recv(sockfd, pe, bodylen, 0)) < 0) {
            send_bad_request(sockfd);
            free(buffer);
            return -1;
        }
        quickack(sockfd);
        bodylen -= n;
        p = cson_parse(cson, p, (pe += n));
    }

    free(buffer);

    return 0;
}

int send_response(const int sockfd, const char *body, const size_t len) {
    char header[512];
    ssize_t n = sprintf(header, "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: application/json\r\nContent-Length: %ld\r\n\r\n", len);
    if (n < 0) {
        return -1;
    }
    if (send(sockfd, header, n, 0) < 0) {
        return -1;
    }
    if (send(sockfd, body, len, 0) < 0) {
        return -1;
    }
    sflush(sockfd);
    quickack(sockfd);
    return 0;
}
