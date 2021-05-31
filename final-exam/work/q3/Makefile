# Makefile for bit-string tester

CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror

bs : bs.o bits.o util.o
	$(CC) -o bs bs.o bits.o util.o

bits.o: bits.c bits.h
util.o: util.c util.h

clean:
	rm -f bs *.o
