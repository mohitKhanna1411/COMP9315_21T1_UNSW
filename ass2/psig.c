// psig.c ... functions on page signatures (psig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include <math.h>
#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "tsig.h"
#include "hash.h"

Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	//TODO
	Page dataPage = getPage(r->dataf, nPages(r) - 1);		 // reading page
	Bits new = newBits(psigBits(r));						 // new bits
	Page currentPage = getPage(r->psigf, nPsigPages(r) - 1); // current page
	Tuple temp = malloc(tupSize(r));						 // temporary tuple

	if (pageNitems(dataPage) != 1)								// check if dataPage has more than 1 page
		getBits(currentPage, pageNitems(currentPage) - 1, new); // get bits of that page if only one page

	char *attr;
	int ctr = 0, k = 0, rand_m = 0;
	strcpy(temp, t);		  // string copy
	attr = strtok(temp, ","); // split the tuple string from ',' and get first attribute

	if (sigType(r) == 's')	 // simc
		while (attr != NULL) // while attribute exists
		{
			orBits(new, codewords(attr, psigBits(r), codeBits(r), psigBits(r))); // generate cw and bitwise OR with new
			attr = strtok(NULL, ",");											 // get next attr
		}
	else if (sigType(r) == 'c') // catc
		while (attr != NULL)	// while attribute exists
		{
			ctr++;
			rand_m = psigBits(r) / nAttrs(r);				   // calculating rand_m
			k = (psigBits(r) / nAttrs(r)) / 2 / maxPsigsPP(r); // calculating value of k

			if (ctr == nAttrs(r)) // check if last attribute
			{
				rand_m = psigBits(r) / nAttrs(r) + (psigBits(r) % nAttrs(r));				// putting reamining extra bits in rand_m
				k = (psigBits(r) / nAttrs(r) + psigBits(r) % nAttrs(r)) / 2 / maxTupsPP(r); // putting reamining extra bits in k
			}
			orBits(new, codewords(attr, psigBits(r), k, rand_m)); // generate cw and bitwise OR with new
			if (ctr != nAttrs(r))								  // do not shift for last attribute
				shiftBits(new, rand_m);							  // left shift for rand_m values to make space for OR in next iteration

			attr = strtok(NULL, ","); // get next attr
		}

	return new;
}
void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	//TODO
	if (sigType(q->rel) == 's') // simc
	{
		Bits tupleSignature = makeTupleSigWithMK(q->rel, q->qstring, psigBits(q->rel), codeBits(q->rel)); // get tuple signature
		for (int i = 0; i < nPsigPages(q->rel); i++)													  // iterate over every page
		{
			Page currentPage = getPage(psigFile(q->rel), i); // reading current page
			q->nsigpages++;									 // incrementing signature pages read

			for (int j = 0; j < pageNitems(currentPage); j++) // iterate over every item in page
			{
				Bits psig = newBits(psigBits(q->rel)); //new bits of size pm of relation q->rel
				getBits(currentPage, j, psig);		   // a bitstrings from jth postion in page buffeer
				q->nsigs++;							   // incrementing signatures read

				if (isSubset(tupleSignature, psig))				  // check if its a match
					setBit(q->pages, i * maxPsigsPP(q->rel) + j); // set bit to 1 at calculated position at each page
			}
		}
	}
	else if (sigType(q->rel) == 'c') // catc
	{
		Bits pageSignature = makePageSig(q->rel, q->qstring); // get page signature
		unsetAllBits(q->pages);								  // set every bit to 0
		for (PageID i = 0; i < nPsigPages(q->rel); i++)		  // iterate over every page
		{
			Page currentPage = getPage(psigFile(q->rel), i); // reading current page
			q->nsigpages++;									 // incrementing signature pages read

			for (int j = 0; j < pageNitems(currentPage); j++) // iterate over every item in page
			{
				Bits psig = newBits(psigBits(q->rel)); //new bits of size pm of relation q->rel
				getBits(currentPage, j, psig);		   // a bitstrings from jth postion in page buffeer

				if (isSubset(pageSignature, psig)) // check if its a match
				{
					PageID p_to_examine = q->nsigs;
					setBit(q->pages, p_to_examine); // set bit to 1 at calculated position at each page
				}
				freeBits(psig); // free
				q->nsigs++;		// increment signatures read
			}
		}
	}
}