###############################################################################
# Makefile for assignment 3, Parallel and Distributed Computing 2022.
###############################################################################

CC = mpicc
CFLAGS = -std=c99 -g -O3
LIBS = -lm
RELEASEFLAGS:=-O3
DEBUGFLAGS:=-g

all: code

code: qsort.c
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -o shearsort ssort.c $(LIBS)

# debug: qsort.c
# 	$(CC) $(CFLAGS) $(DEBUGFLAGS) -D DEBUG -o quicksortD qsort.c $(LIBS)

clean:
	$(RM) quicksort quicksortD

