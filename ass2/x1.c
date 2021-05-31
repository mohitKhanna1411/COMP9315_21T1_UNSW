// Test Bits ADT

#include <stdio.h>
#include "defs.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"

int main(int argc, char **argv)
{
	// Bits b1 = newBits(64);
	Bits b2 = newBits(64);
	// printf("t=1: ");
	// showBits(b2);
	printf("\n");
	printf("t=2: ");
	showBits(b2);
	printf("\n");
	setBit(b2, 0);
	setBit(b2, 6);
	setBit(b2, 7);
	setBit(b2, 9);
	setBit(b2, 19);
	setBit(b2, 20);
	setBit(b2, 21);
	setBit(b2, 22);
	printf("\n");
	printf("t=2: ");
	showBits(b2);
	shiftBits(b2, 16);
	// orBits(b1, b2);
	printf("\n");
	// printf("t=2: ");
	showBits(b2);
	// showBits(b1);
	// 0000000000000000000000000111100000000010110000010000000000000000
	// 0000000000000000000000000000000000000000011110000000001011000001
	// printf("\n");
	// setBit(b, 0);
	// setBit(b, 50);
	// setBit(b, 59);
	// printf("t=2: ");
	// showBits(b);
	// printf("\n");
	// if (bitIsSet(b, 5))
	// 	printf("Bit 5 is set\n");
	// if (bitIsSet(b, 10))
	// 	printf("Bit 10 is set\n");
	// setAllBits(b);
	// printf("t=3: ");
	// showBits(b);
	// printf("\n");
	// unsetBit(b, 40);
	// printf("t=4: ");
	// showBits(b);
	// printf("\n");
	// if (bitIsSet(b, 20))
	// 	printf("Bit 20 is set\n");
	// if (bitIsSet(b, 40))
	// 	printf("Bit 40 is set\n");
	// if (bitIsSet(b, 50))
	// 	printf("Bit 50 is set\n");
	// setBit(b, 59);
	return 0;
}
