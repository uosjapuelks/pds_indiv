###############################################################################
# Makefile for assignment 3, Parallel and Distributed Computing 2022.
###############################################################################

CC = mpicc
CFLAGS = -std=c99 -g -O3
LIBS = -lm
RELEASEFLAGS:=-O3
DEBUGFLAGS:=-g

all: code

code: ssort.c
	$(CC) $(CFLAGS) $(RELEASEFLAGS) -o shear ssort.c $(LIBS)

debug: ssort.c
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -D DEBUG -o dbshear ssort.c $(LIBS)

clean:
	$(RM) shear dbshear

