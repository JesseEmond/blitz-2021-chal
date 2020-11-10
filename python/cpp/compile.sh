#!/bin/bash
set -e
cd cpp

CPPFLAGS='-c -g -Wall -fPIC -Ofast -DNDEBUG'
CXXFLAGS='-std=c++17'
LDFLAGS='-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now'

# use this to generate/use a PGO profile file
#CPPFLAGS="$CPPFLAGS -fprofile-instr-generate"
# Post-processed with:
# llvm-profdata-10 merge --output cpp/default.profdata default.profraw
#CPPFLAGS="$CPPFLAGS -fprofile-instr-use"

echo "Compiling..."
clang++ $CPPFLAGS $CXXFLAGS seqsum.cpp -o seqsum.o
clang $CPPFLAGS $CFLAGS server.c -o server.o

echo "Linking..."
clang++ -shared $LDFLAGS -o libseqsum.so seqsum.o server.o
