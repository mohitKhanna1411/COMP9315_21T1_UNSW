// bsig.c ... functions on Tuple Signatures (bsig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "tsig.h"
#include "psig.h"
#include "hash.h"
#include "bits.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	//TODO

	if (sigType(q->rel) == 's') // simc
	{
		setAllBits(q->pages);																			  // setting all bits to 1
		Bits bsigPages = newBits(nBsigPages(q->rel));													  // new bits of  number of bsig pages of relation q->rel
		Bits tupleSignature = makeTupleSigWithMK(q->rel, q->qstring, psigBits(q->rel), codeBits(q->rel)); // make tuple signatures

		for (int i = 0; i < psigBits(q->rel); i++) // iterate over all pages
			if (bitIsSet(tupleSignature, i))	   // check if bit is set to 1 at ith position
			{
				Page bsPage = getPage(bsigFile(q->rel), iceil(i + 1, maxBsigsPP(q->rel)) - 1); // fetch a Page from file of relation q->rel
				Bits bs = newBits(bsigBits(q->rel));										   // new bits
				q->nsigs++;																	   // increament signatures read

				if (!bitIsSet(bsigPages, iceil(i + 1, maxBsigsPP(q->rel)) - 1)) // bsig pages read record
					setBit(bsigPages, iceil(i + 1, maxBsigsPP(q->rel)) - 1);	// set bit of 1 on calculated position for bits bsigPages

				getBits(bsPage, i % maxBsigsPP(q->rel), bs); // get bitstring from specified position in bsPage Page buffer

				for (int j = 0; j < nbit(q->pages); j++) // iterate over every bit of q->pages
					if (bitIsSet(q->pages, j))			 // check if bit is 1 for q->pages
						if (!bitIsSet(bs, j))			 // check if bit is 0 for the bs
							unsetBit(q->pages, j);		 // unset bit of q->pages

				freeBits(bs); // freeing up the space allocated for bs
			}
		for (int i = 0; i < nBsigPages(q->rel); i++) // iterate over every bsig page
			if (bitIsSet(bsigPages, i))				 // if bit is 1
				q->nsigpages++;						 // increment signature pages read

		freeBits(bsigPages);	  // freeing up the space allocated for bsigPages
		freeBits(tupleSignature); // freeing up the space allocated for tupleSignature
	}
	else if (sigType(q->rel) == 'c') // catc
	{
		PageID bsigPid = -1;						 // pageId
		PageID pbsigPid = -1;						 // pageId
		Bits qsig = makePageSig(q->rel, q->qstring); // get page signature
		setAllBits(q->pages);						 // set all the bits to 1

		for (int i = 0; i < psigBits(q->rel); ++i) // iterate over width of page signature
		{
			Bits slice = newBits(bsigBits(q->rel)); // fetch slice [i]
			if (bitIsSet(qsig, i))					// check if bit is set at ith position
			{
				bsigPid = i / maxBsigsPP(q->rel);			 // cal pageId
				Page p = getPage(bsigFile(q->rel), bsigPid); // get page
				if (bsigPid != pbsigPid)					 // check if not equal
					q->nsigpages++;							 // increase signature pages read
				pbsigPid = bsigPid;							 // assign
				getBits(p, i % maxBsigsPP(q->rel), slice);	 // getbits
				q->nsigs++;									 // increment signatures read
				for (int j = 0; j < nPages(q->rel); ++j)	 // iterate over number of data pages
					if (!bitIsSet(slice, j))				 // check if bit is not set to 1 at jth location
						unsetBit(q->pages, j);				 // set bit to 0 on q->pages at jth location
			}
		}
	}
}
