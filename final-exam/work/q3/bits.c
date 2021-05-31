// bits.c ... functions on bit-strings

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "bits.h"
#include "util.h"

#define BITS_PER_WORD 32

typedef unsigned int Count; // non-negative integer
typedef unsigned int Word;	// unsigned 32-bit integer value

typedef struct _BitsRep
{
	Count nbits;	   // how many bits
	Count nwords;	   // how many Words in array
	Word bitstring[1]; // array of Words to hold bits
					   // actual array size is nwords
} BitsRep;

// create a new Bits object
Bits newBits(int nbits)
{
	Count nwords = iceil(nbits, BITS_PER_WORD);
	Bits new = malloc(2 * sizeof(Count) + nwords * sizeof(Word));
	assert(new != NULL);
	new->nbits = nbits;
	new->nwords = nwords;
	memset(&(new->bitstring[0]), 0x00, nwords * sizeof(Word));
	return new;
}

// release memory associated with a Bits object

void freeBits(Bits b)
{
	free(b);
}

// XXX put any helper functions here XXX

// set the bit at position to 1

void setBit(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	int word = position / BITS_PER_WORD;			// word
	int bit = position % BITS_PER_WORD;				// bit
	Word mask = (1 << bit);							// mask
	b->bitstring[word] = b->bitstring[word] | mask; // set that position to 1 by bitwise OR with mask
}

// set all bits to 1
// including possibly some bits beyond the top-end of the bit-string

void setAllBits(Bits b)
{
	assert(b != NULL);
	memset(&(b->bitstring[0]), 0xFF, b->nwords * sizeof(Word));
}

// set the bit at position to 0

void unsetBit(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	int word = position / BITS_PER_WORD;			// word
	int bit = position % BITS_PER_WORD;				// bit
	Word mask = ~(1 << bit);						// mask bitwise NOT
	b->bitstring[word] = b->bitstring[word] & mask; // set that position to 0 by bitwise AND with mask
}

// set all bits to 0

void unsetAllBits(Bits b)
{
	assert(b != NULL);
	memset(&(b->bitstring[0]), 0x00, b->nwords * sizeof(Word));
}

// show Bits on stdout
// display in order MSB to LSB
// do not append '\n'

void showBits(Bits b)
{
	assert(b != NULL);
	for (int i = b->nwords - 1; i >= 0; i--) // iter over words
	{
		int temp;														// temp
		if (i == b->nwords - 1)											// if it is same as the iter
			temp = b->nwords > 1 ? b->nbits - BITS_PER_WORD : b->nbits; // conditional change for temp
		else
			temp = BITS_PER_WORD;			// stays 32
		for (int j = temp - 1; j >= 0; j--) // iter over temp
		{
			Word mask = (1 << j);		// mask
			if (b->bitstring[i] & mask) // check if encountered 1
				putchar('1');			// print 1
			else						// otherwise
				putchar('0');			// print 0
		}
	}
}
