#
# "makefile" for the Phase 2 encoder/decoder (UVic SENG 265,
# summer 2017, assignment #4)
#

CC=gcc
CFLAGS=-c -g -Wall -ansi -DDEBUG

all: phase2

phase2: phase2.o linkedlist.o
	$(CC) phase2.o linkedlist.o -o phase2

phase2.o: phase2.c linkedlist.h
	$(CC) $(CFLAGS) phase2.c

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) linkedlist.c

clean:
	rm -rf *.o phase2 testlist
