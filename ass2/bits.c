// bits.c ... functions on bit-strings
// part of signature indexed files
// Bit-strings are arbitrarily long byte arrays
// Least significant bits (LSB) are in array[0]
// Most significant bits (MSB) are in array[nbytes-1]

// Written by John Shepherd, March 2019

#include <assert.h>
#include "defs.h"
#include "bits.h"
#include "page.h"
#include "hash.h"
#include "reln.h"

Bits tcw_cact(char *attr, int m, int k, int len_sig)
{
	int nbits = 0;

	Bits codeword = newBits(len_sig);

	srandom(hash_any(attr, strlen(attr)));
	while (nbits < k)
	{
		int i = random() % m;
		if (!bitIsSet(codeword, i))
		{
			setBit(codeword, i);
			nbits++;
		}
	}
	return codeword;
}

Bits codewords(char *attr_value, int m, int k, int rand_m)
{

	int ctr = 0;
	Bits cword = newBits(m);		  // codeword of size m
	if (strcmp(attr_value, "?") == 0) // if wildcard string
		return cword;				  // return empty cw

	srandom(hash_any(attr_value, strlen(attr_value))); // random number
	while (ctr < k)									   // while counter < k
	{
		int i = random() % rand_m; // random position within range 0 to rand_m

		if (!bitIsSet(cword, i)) // check bit is not set to 1 at ith position
		{
			setBit(cword, i); // set bit to 1 at ith position
			ctr++;			  // increment counter
		}
	}
	return cword; // return cw
}

// create a new Bits object

Bits newBits(int nbits)
{
	Count nbytes = iceil(nbits, 8);
	Bits new = malloc(2 * sizeof(Count) + nbytes);
	new->nbits = nbits;
	new->nbytes = nbytes;
	memset(&(new->bitstring[0]), 0, nbytes);
	return new;
}

// release memory associated with a Bits object

void freeBits(Bits b)
{
	//TODO
	free(b); // freeing up the space allocated for b
}

// check if the bit at position is 1

Bool bitIsSet(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	//TODO
	int byte = position / 8; // byte
	int bit = position % 8;	 // bit

	Bool result = FALSE;					 // FALSE
	Byte mask = (1 << bit);					 // create a mask
	Byte *tempBitString = malloc(b->nbytes); // creating a temporary bitstring of size b->nbytes

	memcpy(tempBitString, &(b->bitstring), b->nbytes); // copy b->bitstring to tempBitString

	if (tempBitString[byte] & mask) // bitwise and of mask and tempbitstring[byte]
		result = TRUE;				// TRUE

	free(tempBitString); //freeing up the space allocated for tempBitString
	return result;		 // return 1 (TRUE) is the position is set or else return 0 (FALSE)
}

// check whether one Bits b1 is a subset of Bits b2

Bool isSubset(Bits b1, Bits b2)
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	//TODO
	for (int i = 0; i < b1->nbytes; i++)							   // iterating over the whole bitstring byte-wise
		if ((b1->bitstring[i] & b2->bitstring[i]) != b1->bitstring[i]) // bitwise AND and then check if equal to the bitstring
			return FALSE;											   // return FALSE

	return TRUE; // return TRUE
}

// set the bit at position to 1

void setBit(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	//TODO
	int byte = position / 8; // byte
	int bit = position % 8;	 // bit

	Byte mask = (1 << bit);					 // mask
	Byte *tempBitString = malloc(b->nbytes); // creating a temporary bitstring of size b->nbytes

	memcpy(tempBitString, &(b->bitstring), b->nbytes); // copy b->bitstring to tempBitString
	tempBitString[byte] = tempBitString[byte] | mask;  // set that position to 1 by bitwise OR with mask
	memcpy(&(b->bitstring), tempBitString, b->nbytes); // copy tempBitString to b->bitstring
	free(tempBitString);							   // freeing up the space allocated for tempBitString
}

// set all bits to 1

void setAllBits(Bits b)
{
	assert(b != NULL);
	//TODO
	int byte = b->nbytes; // byte

	for (int i = 0; i < byte; i++)						  // iterating over bitstring bytewise
		for (int j = 0; j < 8; j++)						  // iterating over byte of bitstring bitwise
			b->bitstring[i] = b->bitstring[i] | (1 << j); // bitwise OR to set each bit to 1
}
// set the bit at position to 0

void unsetBit(Bits b, int position)
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	//TODO
	int byte = position / 8; // byte
	int bit = position % 8;	 // bit

	Byte mask = ~(1 << bit);				 // mask bitwise NOT
	Byte *tempBitString = malloc(b->nbytes); // creating a temporary bitstring of size b->nbytes

	memcpy(tempBitString, &(b->bitstring), b->nbytes); // copy b->bitstring to tempBitString
	tempBitString[byte] = tempBitString[byte] & mask;  // set that position to 0 by bitwise AND with mask
	memcpy(&(b->bitstring), tempBitString, b->nbytes); // copy tempBitString to b->bitstring
	free(tempBitString);							   // freeing up the space allocated for tempBitString
}

// set all bits to 0

void unsetAllBits(Bits b)
{
	assert(b != NULL);
	//TODO
	int byte = b->nbytes; // byte

	for (int i = 0; i < byte; i++)						   // iterating over bitstring bytewise
		for (int j = 0; j < 8; j++)						   // iterating over byte of bitstring bitwise
			b->bitstring[i] = b->bitstring[i] & ~(1 << j); // bitwise OR with bitwise NOT to set each bit to 0
}
void editBits(Page p, Offset pos, Bits b)
{
	//TODO
	int size = b->nbytes;
	Byte *addr = addrInPage(p, pos, size);
	// printf("bitstring is:"); showBits(b);printf("\n");
	memcpy(addr, b->bitstring, size);
}
// bitwise AND ... b1 = b1 & b2

void andBits(Bits b1, Bits b2)
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	//TODO
	int byte = b1->nbytes;								 //  byte
	Byte *tempBitString = calloc(1, b1->nbytes);		 // initialize tempBitString and memory by zero
	memcpy(tempBitString, &(b1->bitstring), b1->nbytes); // copy b->bitstring to tempBitString

	for (int i = byte - 1; i >= 0; i--)							// iterating over bitstring bytewise
		tempBitString[i] = b1->bitstring[i] & b2->bitstring[i]; // bitwise AND bitstrings of b1 and b2 to store in tempBitString

	memcpy(&(b1->bitstring), tempBitString, b1->nbytes); // copy tempBitString to b->bitstring
	free(tempBitString);								 // freeing up the space allocated for tempBitString
}

// bitwise OR ... b1 = b1 | b2

void orBits(Bits b1, Bits b2)
{
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	//TODO
	int byte = b1->nbytes;								 //  byte
	Byte *tempBitString = calloc(1, b1->nbytes);		 // initialize tempBitString and memory by zero
	memcpy(tempBitString, &(b1->bitstring), b1->nbytes); // copy b->bitstring to tempBitString

	for (int i = byte - 1; i >= 0; i--)							// iterating over bitstring bytewise
		tempBitString[i] = b1->bitstring[i] | b2->bitstring[i]; // bitwise OR bitstrings of b1 and b2 to store in tempBitString

	memcpy(&(b1->bitstring), tempBitString, b1->nbytes); // copy tempBitString to b->bitstring
	free(tempBitString);								 // freeing up the space allocated for tempBitString
}

// left-shift ... b1 = b1 << n
// negative n gives right shift

void shiftBits(Bits b, int n)
{
	//TODO
	assert(b != NULL);
	int byte = b->nbytes;			   // byte
	int bit = b->nbits;				   // bit
	int ctr = 0;					   // counter to track position
	Bits tempBitString = newBits(bit); // tempBitString bits of size bit

	for (int i = 0; i <= byte - 1; i++) // iterating over bytes from left to right
		for (int j = 0; j <= 7; j++)	// iterating over bits from left to right
		{
			Byte mask = (1 << j);				// mask
			if (b->bitstring[i] & mask)			// check if the encountered bit is 1
				setBit(tempBitString, ctr + n); // set 1 at (ctr + n)th position of tempBitString

			if (ctr == bit - 1 - n) // check when to stop reading b->bitstring
				break;				// break

			ctr++; // increment position
		}

	memcpy(&(b->bitstring), tempBitString->bitstring, tempBitString->nbytes); // copy tempBitString->bitstring to b->bitstring
	free(tempBitString);													  // freeing up the space allocated for tempBitString
}

// get a bit-string (of length b->nbytes)
// from specified position in Page buffer
// and place it in a BitsRep structure

void getBits(Page p, Offset pos, Bits b)
{
	//TODO
	memcpy(&(b->bitstring[0]), addrInPage(p, pos, b->nbytes), b->nbytes); // get absolute pointer/address for items of
																		  // Page p and copy that to b->bitstring
}

// copy the bit-string array in a BitsRep
// structure to specified position in Page buffer

void putBits(Page p, Offset pos, Bits b)
{
	//TODO
	memcpy(addrInPage(p, pos, b->nbytes), &(b->bitstring[0]), b->nbytes); // copy b->bitstring of size b->nbytes to the
																		  //specified position in Page buffer
}

// show Bits on stdout
// display in order MSB to LSB
// do not append '\n'

void showBits(Bits b)
{
	assert(b != NULL);
	// printf("(%d,%d)", b->nbits, b->nbytes);
	for (int i = b->nbytes - 1; i >= 0; i--)
	{
		for (int j = 7; j >= 0; j--)
		{
			Byte mask = (1 << j);
			if (b->bitstring[i] & mask)
				putchar('1');
			else
				putchar('0');
		}
	}
}

unsigned int nbit(Bits b) { return b->nbits; } // returns the size in  bits

unsigned int nbyte(Bits b) { return b->nbytes; } // returns the size in bytes