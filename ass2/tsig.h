// tsig.h ... interface to functions on tuple signatures
// part of signature indexed files
// Written by John Shepherd, March 2019

#ifndef TSIG_H
#define TSIG_H 1

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "bits.h"

Bits makeTupleSig(Reln, Tuple);
Bits makeTupleSigWithMK(Reln, Tuple, int, int);
void findPagesUsingTupSigs(Query);

#endif
