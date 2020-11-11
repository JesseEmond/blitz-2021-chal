CC=clang
CXX=clang++

CFLAGS=-g -Wall -fPIC -Ofast -DNDEBUG
CXXFLAGS=$(CFLAGS) -std=c++17
LDFLAGS=-shared

# use this to generate/use a PGO profile file
#PROFFLAGS=-fprofile-instr-generate
# Post-processed with:
# llvm-profdata-10 merge --output cpp/default.profdata default.profraw
#PROFFLAGS=-fprofile-instr-use

all: libseqsum.so

clean:
	rm -f *.o *.so

libseqsum.so: seqsum.o server.o
	$(CXX) -o $@ $(LDFLAGS) $(PROFFLAGS) $^

seqsum.o: seqsum.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(PROFFLAGS) -o $@ $<

server.o: server.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(PROFFLAGS) -o $@ $<
