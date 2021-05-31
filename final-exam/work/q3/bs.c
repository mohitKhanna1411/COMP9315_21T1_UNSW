// Test Bits ADT

#include <stdlib.h>
#include <stdio.h>
#include "bits.h"
#include "util.h"

#define USAGE "Usage:  ./bs  Nbits  +|-  mods"

int main(int argc, char **argv)
{
	if (argc < 3) fatal("Insufficient args",USAGE);

	int nbits = atoi(argv[1]);
	if (nbits < 1) fatal("Need at least 1 bit",USAGE);

	Bits b = newBits(nbits);

	char init = argv[2][0];
	switch (init) {
	case '+': setAllBits(b); break;
	case '-': unsetAllBits(b); break;
	default : fatal("Invalid init",USAGE); break;
	}
	
	printf("init: "); showBits(b); printf("\n");

	int bit; char mod;
	for (int i = 3; i < argc; i++) {
		sscanf(argv[i], "%d", &bit);
		mod = '+';
		if (argv[i][0] == '-') { mod ='-'; bit = 0-bit; }
		if (bit < 0 || bit > nbits-1) {
			printf("Invalid bit position\n");
			continue;
		}
		switch (mod) {
		case '+': setBit(b,bit); putchar('+'); break;
		case '-': unsetBit(b,bit); putchar('-'); break;
		}
		printf("%03d: ", bit); showBits(b); printf("\n");
	}
	return 0;
}
