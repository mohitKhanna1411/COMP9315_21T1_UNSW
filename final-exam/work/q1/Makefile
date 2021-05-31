# Makefile for "pages"

CC=gcc
CFLAGS=-Wall -Werror

pages : pages.o hash.o bits.o
	gcc -o pages pages.o hash.o bits.o

clean:
	rm -f pages *.o
