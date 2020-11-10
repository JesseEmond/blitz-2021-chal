CC=clang
CXX=clang++

CFLAGS=-g -Wall -fPIC -Ofast -DNDEBUG
CXXFLAGS=$(CFLAGS) -std=c++17
LDFLAGS=

# use this to generate/use a PGO profile file
#CFLAGS+=-fprofile-instr-generate
# Post-processed with:
# llvm-profdata-10 merge --output cpp/default.profdata default.profraw
#CFLAGS+=-fprofile-instr-use

all: libseqsum.so

clean:
	rm -f *.o *.so

libseqsum.so: seqsum.o server.o
	$(CXX) -shared -o $@ $(LDFLAGS) $^

seqsum.o: seqsum.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

server.o: server.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<
