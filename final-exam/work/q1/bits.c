// Manipulate 32-bit unsigned values

#include <assert.h>
#include "bits.h"

// check if the bit at position is 1
int bitIsSet(uint32 val, int position)
{
	assert(0 <= position && position < 31);
	uint32 mask = (1 << position);
	return ((val & mask) != 0);
}

// set the bit at position to 1
uint32 setBit(uint32 val, int position)
{
	assert(0 <= position && position < 31);
	uint32 mask = (1 << position);
	return (val | mask);
}

// set the bit at position to 0
uint32 unsetBit(uint32 val, int position)
{
	assert(0 <= position && position < 31);
	uint32 mask = (~(1 << position));
	return (val & mask);
}

// convert 32-bit unsigned quantity to string
char *showBits(uint32 val, char *buf)
{
	int i, j;
	char ch;
	uint32 bit = 0x80000000;

	i = j = 0;
	while (bit != 0)
	{
		ch = ((val & bit) != 0) ? '1' : '0';
		buf[i++] = ch;
		j++;
		if (j % 8 == 0)
			buf[i++] = ' ';
		bit = bit >> 1;
	}
	buf[--i] = '\0';
	return buf;
}
