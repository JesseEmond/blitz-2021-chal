CC := gcc
RL := ragel
LD := gcc

# The challenge VM runs with a 1 core 2 thread Xeon Platinum 8259CL, that's a Cascade Lake chip
# Switch to -march=native if the output doesn't run on your arch, revert for challenge build
CFLAGS = -g -Wall -Ofast -fPIC -flto -frename-registers -msse -msse2 -msse3 -mmmx -m3dnow -DNDEBUG -march=cascadelake
CCFLAGS = $(CFLAGS)
RLFLAGS = -G2
LDFLAGS = -shared

# Generate profile data, run with `python application.py -e 1` to allow clean exit
#PROFFLAGS += -fprofile-generate
# Run `make clean`, then with this one to use PGO
#PROFFLAGS += -fprofile-use

SRCS = \
	cson.c \
	server.c \
	main.c

OBJS = $(SRCS:%=%.o)

TARGET = libseqsum.so

all: $(TARGET)

clean:
	rm -f $(TARGET) *.o

scrub: clean
	rm -f cson.c *.gcda

$(TARGET): $(OBJS)
	$(LD) $(CFLAGS) $(LDFLAGS) $(PROFFLAGS) $^ -o $@

%.c.o: %.c
	$(CC) $(CCFLAGS) $(PROFFLAGS) -c $< -o $@

cson.c: cson.rl
	$(RL) $(RLFLAGS) -o $@ $<
