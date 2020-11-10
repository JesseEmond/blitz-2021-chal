#!/bin/bash
set -e
cd cpp
echo "Compiling..."

options='-c -g -Wall -fPIC -Ofast -std=c++17 -DNDEBUG'

# use this to generate/use a PGO profile file
#all_options='-fprofile-instr-generate'
# Post-processed with:
# llvm-profdata-10 merge --output cpp/default.profdata default.profraw
#all_options='-fprofile-instr-use'

clang++ $all_options $options seqsum.cpp -o seqsum.o

echo "Linking..."
clang++ -g $all_options -shared -Wl,-soname,libseqsum.o -o libseqsum.so seqsum.o