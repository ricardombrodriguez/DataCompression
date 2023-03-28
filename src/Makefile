CC = g++
CFLAGS = -Wall -g

OBJS =
cpm_OBJS = $(OBJS)
cpm_gen_OBJS = $(OBJS)


all: cpm	cpm_gen

cpm:	cpm.o $(cpm_OBJS)
	$(CC) -o ../bin/cpm $^ -lm -g

cpm_gen:	cpm_gen.o $(cpm_gen_OBJS)
	$(CC) -o ../bin/cpm_gen $^ -lm -g
clean:
	rm -rf *.o
