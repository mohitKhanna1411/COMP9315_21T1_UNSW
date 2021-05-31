
-- the input function 'intset_in' takes a null-terminated string (the
-- textual representation of the type) and turns it into the internal
-- (in memory) representation. You will get a message telling you 'intset'
-- does not exist yet but that's okay.

CREATE FUNCTION intset_in(cstring)
   RETURNS intset
   AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;

-- the output function 'intset_out' takes the internal representation and
-- converts it into the intSet representation.

CREATE FUNCTION intset_out(intset)
   RETURNS cstring
   AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE intset (
    input = intset_in,
    output = intset_out,
    internallength = VARIABLE
    -- alignment = int
);

-----------------------------
-- Creating an operator for the new type:
-----------------------------

/*
 * intset_contains function
 * i ? S intSet S contains the integer i
 * That is, i ∈ S
*/
CREATE FUNCTION intset_contains(int, intset)
    RETURNS bool
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR ? (
    leftarg = int,
    rightarg = intset,
    procedure = intset_contains,
    commutator = ?
);

/*
 * intset_cardinality function
 * # S Give the cardinality, or number of distinct elements in, intSet S
 * That is, |S|
*/
CREATE FUNCTION intset_cardinality(intset)
    RETURNS INT
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR # (
    rightarg = intset,
    procedure = intset_cardinality
);

/*
 * intset_subset function
 * A @< B does intSet A contain only values in intSet B
 * That is, the improper subset (A ⊆ B)
*/
CREATE FUNCTION intset_subset(intset, intset)
    RETURNS bool
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR @< (
    leftarg = intset,
    rightarg = intset,
    procedure = intset_subset,
    commutator = @< 
);

/*
 * intset_superset function
 * A >@ B does intSet A contain all the values in intSet B
 * That is, the improper superset (A ⊇ B)
*/
CREATE FUNCTION intset_superset(intset, intset)
    RETURNS bool
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR >@ (
    leftarg = intset,
    rightarg = intset,
    procedure = intset_superset,
    commutator = >@ 
);

/*
 * intset_equal function
 * A = B intSets A and B are equal
 * That is, every element in A can be found in B, and vice versa
*/
CREATE FUNCTION intset_equal(intset, intset)      
    RETURNS bool
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR = (
    leftarg = intset,
    rightarg = intset,
    procedure = intset_equal,
    commutator = = 
);

/*
 * intset_not_equal function
 * A <> B intSets A and B are equal
 * That is, some element in A cannot be found in B, or vice versa
*/
CREATE FUNCTION intset_not_equal(intset, intset)      
    RETURNS bool
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR <> (
    leftarg = intset,
    rightarg = intset,
    procedure = intset_not_equal,
    commutator = <> 
);

/*
 * intset_intersection function
 * A && B Takes the set intersection, and produces an intSet containing the elements common to A and B
 * That is, A ∩ B
*/
CREATE FUNCTION intset_intersection(intset, intset)
    RETURNS intset
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR && (
    leftarg = intset,
    rightarg = intset,
    procedure = intset_intersection,
    commutator = &&
);

/*
 * intset_union function
 * A || B takes the set union, and produces an intSet containing all the elements of A and B
 * That is, A ∪ B
*/
CREATE FUNCTION intset_union(intset, intset)
    RETURNS intset
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR || (
    leftarg = intset,
    rightarg = intset,
    procedure = intset_union,
    commutator = ||
);

/*
 * intset_disjunction function
 * A !! B Takes the set disjunction, and produces an intSet containing elements that are in A and not in B,
 * or that are in B and not in A
*/
CREATE FUNCTION intset_disjunction(intset, intset)
    RETURNS intset
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR !! (
    leftarg = intset,
    rightarg = intset,
    procedure = intset_disjunction,
    commutator = !!
);

/*
 * intset_difference function
 * A - B Takes the set difference, and produces an intSet containing elements that are in A and not in B
*/
CREATE FUNCTION intset_difference(intset, intset)
    RETURNS intset
    AS '/Users/mohitkhanna/Documents/UNSW/21T1/COMP9315/postgresql-12.5/src/tutorial/intset'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR - (
    leftarg = intset,
    rightarg = intset,
    procedure = intset_difference,
    commutator = -
);