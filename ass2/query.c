// query.c ... query scan functions
// part of signature indexed files
// Manage creating and using Query objects
// Written by John Shepherd, March 2019

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"
#include "tsig.h"
#include "psig.h"
#include "bsig.h"
#include "page.h"

// check whether a query is valid for a relation
// e.g. same number of attributes

int checkQuery(Reln r, char *q)
{
	if (*q == '\0')
		return 0;
	char *c;
	int nattr = 1;
	for (c = q; *c != '\0'; c++)
		if (*c == ',')
			nattr++;
	return (nattr == nAttrs(r));
}

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q, char sigs)
{
	Query new = malloc(sizeof(QueryRep));
	assert(new != NULL);
	if (!checkQuery(r, q))
		return NULL;
	new->rel = r;
	new->qstring = q;
	new->nsigs = new->nsigpages = 0;
	new->ntuples = new->ntuppages = new->nfalse = 0;
	new->pages = newBits(nPages(r));
	switch (sigs)
	{
	case 't':
		findPagesUsingTupSigs(new);
		break;
	case 'p':
		findPagesUsingPageSigs(new);
		break;
	case 'b':
		findPagesUsingBitSlices(new);
		break;
	default:
		setAllBits(new->pages);
		break;
	}
	new->curpage = 0;
	return new;
}

// return TRUE if string matches else FALSE
Bool tupleStringcmp(Reln r, Tuple t, char *qstring)
{
	char **tupVals = tupleVals(r, t); // tuple values
	char *pt;						  // substring
	char temp[strlen(qstring)];		  // temp
	strcpy(temp, qstring);			  // temp string
	int ctr = 0;					  // counter
	pt = strtok(temp, ",");			  // string split from ","

	while (pt != NULL) // iterate untill the string is not null
	{
		char *val = tupVals[ctr];	  // value
		if (strcmp(pt, "?") != 0)	  // check if wild card
			if (strcmp(val, pt) != 0) // check not matches
				return FALSE;		  // return false
		pt = strtok(NULL, ",");		  // next string split from ","
		ctr++;						  // increament ctr
	}
	return TRUE; // return 1
}

// scan through selected pages (q->pages)
// search for matching tuples and show each
// accumulate query stats
void scanAndDisplayMatchingTuples(Query q)
{
	assert(q != NULL);
	// TODO
	int matches = 0;		// match counter
	int n = nbit(q->pages); // get size in bits

	for (int i = 0; i < n; i++) // iterate over all the bits
	{
		q->curpage = i;
		if (!bitIsSet(q->pages, i))						// check if bit is set to 1
			continue;									// continue
		Page p = getPage(dataFile(q->rel), q->curpage); // get page
		q->ntuppages++;									// increment data pages read
		for (int j = 0; j < pageNitems(p); j++)			// iterate over all items in this page
		{
			q->curtup = j;									  // tuple
			Tuple t = getTupleFromPage(q->rel, p, q->curtup); // get data for tuple within page
			q->ntuples++;									  // increment tuples examined

			if (tupleStringcmp(q->rel, t, q->qstring)) //check if tuple T = q->qstring
			{
				showTuple(q->rel, t); // print tuple string
				matches++;			  // increament matches
			}
		}
		if (matches == 0) // check doesnt match
			q->nfalse++;  // increment false matches
	}
}

// print statistics on query

void queryStats(Query q)
{
	printf("# sig pages read:    %d\n", q->nsigpages);
	printf("# signatures read:   %d\n", q->nsigs);
	printf("# data pages read:   %d\n", q->ntuppages);
	printf("# tuples examined:   %d\n", q->ntuples);
	printf("# false match pages: %d\n", q->nfalse);
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	free(q->pages);
	free(q);
}
