// bits.h ... interface to functions on bit-strings

#ifndef BITS_H
#define BITS_H 1

typedef struct _BitsRep *Bits;

Bits newBits(int);
void freeBits(Bits);
void setBit(Bits, int);
void setAllBits(Bits);
void unsetBit(Bits, int);
void unsetAllBits(Bits);
void showBits(Bits);

#endif
