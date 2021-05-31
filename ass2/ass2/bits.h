// bits.h ... interface to functions on bit-strings
// part of signature indexed files
// See bits.c for details of functions
// Written by John Shepherd, March 2019

#ifndef BITS_H
#define BITS_H 1
// make a tuple signature

#include "defs.h"
#include "page.h"
typedef struct _BitsRep
{
    Count nbits;       // how many bits
    Count nbytes;      // how many bytes in array
    Byte bitstring[1]; // array of bytes to hold bits
                       // actual array size is nbytes
} BitsRep;
typedef struct _BitsRep *Bits;
Bits newBits(int);
void freeBits(Bits);
Bool bitIsSet(Bits, int);
Bool isSubset(Bits, Bits);
void setBit(Bits, int);
void setAllBits(Bits);
void unsetBit(Bits, int);
void unsetAllBits(Bits);
void andBits(Bits, Bits);
void orBits(Bits, Bits);
void shiftBits(Bits, int);
void getBits(Page, Offset, Bits);
void putBits(Page, Offset, Bits);
void showBits(Bits);
void editBits(Page, Offset, Bits);
Bits codewords(char *, int, int, int);
Bits tcw_cact(char *, int, int, int);
unsigned int nbit(Bits);
unsigned int nbyte(Bits);

#endif
