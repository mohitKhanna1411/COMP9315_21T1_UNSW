// tsig.c ... functions on Tuple Signatures (tsig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"
#include "math.h"
// #include "bits.c"

Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	//TODO
	Bits new = newBits(tsigBits(r)); // create new bits of size tm
	char *attr;
	int ctr = 0, k = 0, rand_m = 0;
	Tuple tempTuple = malloc(tupSize(r)); // temporary tuple
	strcpy(tempTuple, t);				  // string copy
	attr = strtok(tempTuple, ",");		  // split the tuple string from ',' and get first attribute

	if (sigType(r) == 's')	 // simc
		while (attr != NULL) // while attribute exists
		{
			orBits(new, codewords(attr, tsigBits(r), codeBits(r), tsigBits(r))); // generate cw and bitwise OR with new
			attr = strtok(NULL, ",");											 // get next attr
		}
	else if (sigType(r) == 'c') // catc
		while (attr != NULL)	// while attribute exists
		{
			ctr++;
			rand_m = floor(tsigBits(r) / nAttrs(r));  // calculating rand_m
			k = floor((tsigBits(r) / nAttrs(r))) / 2; // calculating value of k

			if (ctr == nAttrs(r)) // check if last attribute
			{
				rand_m = floor(tsigBits(r) / nAttrs(r)) + (tsigBits(r) % nAttrs(r));	// putting reamining extra bits in rand_m
				k = (floor((tsigBits(r) / nAttrs(r))) + (tsigBits(r) % nAttrs(r))) / 2; // putting reamining extra bits in k
			}
			orBits(new, codewords(attr, tsigBits(r), k, rand_m)); // generate cw and bitwise OR with new
			if (ctr != nAttrs(r))								  // do not shift for last attribute
				shiftBits(new, rand_m);							  // left shift for rand_m values to make space for OR in next iteration

			attr = strtok(NULL, ","); // get next attr
		}

	return new; // return new
}

// function to make tuple signatures with given m and k
Bits makeTupleSigWithMK(Reln r, Tuple t, int m, int k)
{
	assert(r != NULL && t != NULL);
	Bits new = newBits(m); // new bits
	char *attr;
	Tuple temp = malloc(tupSize(r));
	strcpy(temp, t);
	attr = strtok(temp, ",");
	if (sigType(r) == 's') // simc

		while (attr != NULL)
		{
			orBits(new, codewords(attr, m, k, m));
			attr = strtok(NULL, ",");
		}
	else if (sigType(r) == 'c') // catc
	{
		int ctr = 0, k = 0, rand_m = 0;
		while (attr != NULL) // while attribute exists
		{
			ctr++;
			rand_m = floor(psigBits(r) / nAttrs(r));				 // calculating rand_m
			k = ceil(psigBits(r) / (nAttrs(r) * maxPsigsPP(r) * 2)); // calculating value of k

			if (ctr == nAttrs(r)) // check if last attribute
			{
				rand_m = floor(psigBits(r) / nAttrs(r) + (psigBits(r) % nAttrs(r)));									   // putting reamining extra bits in rand_m
				k = ceil(((psigBits(r) / (nAttrs(r) * maxPsigsPP(r))) + (psigBits(r) % (nAttrs(r) * maxPsigsPP(r)))) / 2); // putting reamining extra bits in k
			}
			orBits(new, codewords(attr, psigBits(r), k, rand_m)); // generate cw and bitwise OR with new
			if (ctr != nAttrs(r))								  // do not shift for last attribute
				shiftBits(new, rand_m);							  // left shift for rand_m values to make space for OR in next iteration

			attr = strtok(NULL, ","); // get next attr
		}
	}
	return new;
}

// find "matching" pages using tuple signatures

void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);
	//TODO
	Bits querytsig = makeTupleSig(q->rel, q->qstring); // get the tuple signature
	for (int i = 0; i < nTsigPages(q->rel); i++)	   // iterate over pages of relation q->rel
	{
		Page currentPage = getPage(tsigFile(q->rel), i); // currentpage
		q->nsigpages++;									 // increment sig pages ctr

		for (int j = 0; j < pageNitems(currentPage); j++) // iterate over items in page
		{
			Bits tsig = newBits(tsigBits(q->rel)); // create tsig bits of size tm of relation q->rel
			getBits(currentPage, j, tsig);		   // get item of currentpage at position j and place it in tsig bits
			q->nsigs++;							   // increment sig ctr

			if (isSubset(querytsig, tsig)) // check for the match
			{
				int queryIndex = i * maxTsigsPP(q->rel) + j + 1;			// index
				int queryPageId = iceil(queryIndex, maxTupsPP(q->rel)) - 1; // pageId
				setBit(q->pages, queryPageId);								// set bit to 1 at queryPageId position
			}
		}
	}

	// The printf below is primarily for debugging
	// Remove it before submitting this function
	// printf("Matched Pages:");
	// showBits(q->pages);
	// putchar('\n');
}
