CC := gcc
CXX := g++
RL := ragel
LD := g++

CFLAGS = -g -Wall -fPIC -Ofast -DNDEBUG
CCFLAGS = $(CFLAGS)
CXXFLAGS = $(CFLAGS) -std=c++17
RLFLAGS = -G2
LDFLAGS = -shared -lpthread

# use this to generate/use a PGO profile file
#PROFFLAGS = -fprofile-instr-generate
# Post-processed with:
# llvm-profdata-10 merge --output cpp/default.profdata default.profraw
#PROFFLAGS = -fprofile-instr-use

SRCS = \
	cson.c \
	server.c \
	main.c

OBJS = $(SRCS:%=%.o)

TARGET = libseqsum.so

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) cson.c

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(PROFFLAGS) $^ -o $@

%.c.o: %.c
	$(CC) $(CCFLAGS) $(PROFFLAGS) -c $< -o $@

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PROFFLAGS) -c $< -o $@

cson.c: cson.rl
	$(RL) $(RLFLAGS) -o $@ $<
