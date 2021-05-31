/*
 * COMP9315 Database Implementation
 * Assignment 1
 * Mohit Khanna
 * z5266543
*/

/*
 * Headers
*/
#include "postgres.h"
#include "fmgr.h"
#include "c.h"
#include <stdlib.h>

/*
 * macros
*/
#define false 0
#define true 1
#define size_16 16
#define size_1024 1024

PG_MODULE_MAGIC;

/*
 * struct declarations
*/
typedef struct _IntSet
{
    int32 length;
    int data[FLEXIBLE_ARRAY_MEMBER];
} intSet;
typedef struct _Node
{
    int value;
    int exist;
    struct _Node *next;
} node;

/*
 * user defined function declarations
*/
node *createEmptyNode(void);
node *find(node *n, int value);
node *insert(node *n, int val, int *size);
int isValidInteger(int *out, char *s);
int processSubString(char *s, int start, int end, int *out);
int getHashValue(int val);
int processString(node *n, char *str, int *size);
int trim(char *str, char *input);
int *convertIntoIntArr(node *n, int size);
int partition(int arr[], int low, int high);
void qSort(int arr[], int start, int end);
int binarySearch(int arr[], int left, int right, int x);

/*
 *intset_in function
*/
PG_FUNCTION_INFO_V1(intset_in);

Datum
    intset_in(PG_FUNCTION_ARGS)
{
    int size = 0;
    int *arr;
    intSet *t;
    node *n = createEmptyNode();

    char *str = PG_GETARG_CSTRING(0);

    if (!processString(n, str, &size))
        ereport(ERROR,
                (errcode(ERRCODE_FDW_INVALID_STRING_FORMAT),
                 errmsg("invalid input syntax for type intset: \"%s\"",
                        str)));

    arr = convertIntoIntArr(n, size);                    // convert into int array with unqiue values only
    qSort(arr, 0, size - 1);                             // sorting the array using quick sort with O(nlogn) in best case
    t = (intSet *)palloc(VARHDRSZ + size * sizeof(int)); // macro VARHDRSZ to refer to the size of the overhead for a variable length type.
    SET_VARSIZE(t, VARHDRSZ + size * sizeof(int));
    memcpy(t->data, arr, size * sizeof(int));
    free(arr);
    PG_RETURN_POINTER(t);
}

/*
 *intset_out function
*/
PG_FUNCTION_INFO_V1(intset_out);

Datum
    intset_out(PG_FUNCTION_ARGS)
{
    intSet *t = (intSet *)PG_GETARG_POINTER(0);
    int size = (t->length) / size_16 - 1;
    char *str = "{"; // start with '{'
    int i;
    if (size == 0)               // if size is 0
        PG_RETURN_CSTRING("{}"); // return just the curly braces

    for (i = 0; i < size; i++)
    {
        if (i == 0)
            str = psprintf("%s%d", str, t->data[i]);  //Format text data under the control of fmt (an sprintf-style format string).
                                                      // * and return it in an allocated-on-demand buffer
        else                                          // handling the commas ','
            str = psprintf("%s,%d", str, t->data[i]); // appending the integers
    }
    str = psprintf("%s}", str); // append the last '}'
    PG_RETURN_CSTRING(str);
}

/*
 * intset_contains function
 * i ? S intSet S contains the integer i
 * That is, i ∈ S
*/
PG_FUNCTION_INFO_V1(intset_contains);

Datum
    intset_contains(PG_FUNCTION_ARGS)
{
    long a = (long)PG_GETARG_POINTER(0);
    intSet *t = (intSet *)PG_GETARG_POINTER(1);
    int size = (t->length) / size_16 - 1; // calculate the size of the intset

    PG_RETURN_BOOL(size <= 0 ? false : binarySearch(t->data, 0, size - 1, a) != -1); //returns true if the data is found and size > 0
                                                                                     //returns false if the data is not found and size > 0
                                                                                     //returns false if size = 0
}

/*
 * intset_cardinality function
 * # S Give the cardinality, or number of distinct elements in, intSet S
 * That is, |S|
*/
PG_FUNCTION_INFO_V1(intset_cardinality);

Datum
    intset_cardinality(PG_FUNCTION_ARGS)
{
    intSet *t = (intSet *)PG_GETARG_POINTER(0);
    PG_RETURN_INT32(t->length / size_16 - 1); //returns length of intset t
}

/*
 * intset_superset function
 * A >@ B does intSet A contain all the values in intSet B
 * That is, the improper superset (A ⊇ B)
*/
PG_FUNCTION_INFO_V1(intset_superset);
Datum
    intset_superset(PG_FUNCTION_ARGS)
{
    intSet *t1 = (intSet *)PG_GETARG_POINTER(0);
    intSet *t2 = (intSet *)PG_GETARG_POINTER(1);
    int sizeT1 = (t1->length) / size_16 - 1;
    int sizeT2 = (t2->length) / size_16 - 1;
    int i, r;
    if (sizeT2 > sizeT1)         // if length of t2 > length of t1
        PG_RETURN_BOOL(false);   // return 0
    if (sizeT2 == 0)             // if t2 has nothing
        PG_RETURN_BOOL(true);    // return 1
    for (i = 0; i < sizeT2; i++) // loop over every element of t2
    {
        r = binarySearch(t1->data, 0, sizeT1 - 1, t2->data[i]); // binary search element of t2 in t1 array
        if (r == -1)
            PG_RETURN_BOOL(false); // return 0 if any element is not found
    }
    PG_RETURN_BOOL(true); // returns 1 if every element of t2 is in t1
}

/*
 * intset_subset function
 * A @< B does intSet A contain only values in intSet B
 * That is, the improper subset (A ⊆ B)
*/
PG_FUNCTION_INFO_V1(intset_subset);
Datum
    intset_subset(PG_FUNCTION_ARGS)
{
    intSet *t1 = (intSet *)PG_GETARG_POINTER(0);
    intSet *t2 = (intSet *)PG_GETARG_POINTER(1);
    int sizeT1 = (t1->length) / size_16 - 1;
    int sizeT2 = (t2->length) / size_16 - 1;
    int i, r;
    if (sizeT1 > sizeT2)         // if length of t2 > length of t1
        PG_RETURN_BOOL(false);   //return 0
    if (sizeT1 == 0)             //if t1 has nothing
        PG_RETURN_BOOL(true);    // return 1
    for (i = 0; i < sizeT1; i++) // loop over every element of t1
    {
        r = binarySearch(t2->data, 0, sizeT2 - 1, t1->data[i]); // binary search element of t1 in t2 array
        if (r == -1)
            PG_RETURN_BOOL(false); // return 0 if any element is not found
    }
    PG_RETURN_BOOL(true); // returns 1 if every element of t1 is in t2
}

/*
 * intset_equal function
 * A = B intSets A and B are equal
 * That is, every element in A can be found in B, and vice versa
*/
PG_FUNCTION_INFO_V1(intset_equal);
Datum
    intset_equal(PG_FUNCTION_ARGS)
{
    intSet *t1 = (intSet *)PG_GETARG_POINTER(0);
    intSet *t2 = (intSet *)PG_GETARG_POINTER(1);
    int sizeT1 = (t1->length) / size_16 - 1;
    int sizeT2 = (t2->length) / size_16 - 1;
    if (sizeT1 != sizeT2)      // if sizes are different
        PG_RETURN_BOOL(false); // return 0
    if (sizeT1 == 0)           // handling '{}' = '{}'
        PG_RETURN_BOOL(true);
    for (int i = 0; i < sizeT1; i++)
    {
        if (t1->data[i] != t2->data[i]) // if element of t1 not equal to element of t2
            PG_RETURN_BOOL(false);      // return 0
    }
    PG_RETURN_BOOL(true); // return 1
}

/*
 * intset_not_equal function
 * A <> B intSets A and B are equal
 * That is, some element in A cannot be found in B, or vice versa
*/
PG_FUNCTION_INFO_V1(intset_not_equal);
Datum
    intset_not_equal(PG_FUNCTION_ARGS)
{
    intSet *t1 = (intSet *)PG_GETARG_POINTER(0);
    intSet *t2 = (intSet *)PG_GETARG_POINTER(1);
    int sizeT1 = (t1->length) / size_16 - 1;
    int sizeT2 = (t2->length) / size_16 - 1;
    if (sizeT1 != sizeT2)     // if sizes are different
        PG_RETURN_BOOL(true); //return 1
    if (sizeT1 == 0)          // handling '{}' <> '{}'
        PG_RETURN_BOOL(false);
    for (int i = 0; i < sizeT1; i++)
    {
        if (t1->data[i] != t2->data[i]) // if element of t1 not equal to t2
            PG_RETURN_BOOL(true);       //return 1
    }
    PG_RETURN_BOOL(false); // return 0
}

/*
 * intset_intersection function
 * A && B Takes the set intersection, and produces an intSet containing the elements common to A and B
 * That is, A ∩ B
*/
PG_FUNCTION_INFO_V1(intset_intersection);
Datum
    intset_intersection(PG_FUNCTION_ARGS)
{
    intSet *t1 = (intSet *)PG_GETARG_POINTER(0);
    intSet *t2 = (intSet *)PG_GETARG_POINTER(1);
    intSet *t;
    int sizeT1 = (t1->length) / size_16 - 1;
    int sizeT2 = (t2->length) / size_16 - 1;
    int i = 0, j = 0, k = 0;

    int *arr = malloc((sizeT1 + sizeT2) * sizeof(int));
    while (i < sizeT1 && j < sizeT2) // iterating over t1 and t2
    {
        if (t1->data[i] < t2->data[j])      // if element t1 < element t2
            i++;                            // increment t1 iterator
        else if (t2->data[j] < t1->data[i]) // if element t2 < element t1
            j++;                            // increment t2 iterator
        else                                // element t1 = element t2
        {
            arr[k++] = t1->data[i++]; // insert either and increment both interators
            j++;
        }
    }

    t = (intSet *)palloc(VARHDRSZ + k * sizeof(int)); // macro VARHDRSZ to refer to the size of the overhead for a variable length type.
    SET_VARSIZE(t, VARHDRSZ + k * sizeof(int));
    memcpy(t->data, arr, k * sizeof(int));
    free(arr); // free temp array
    PG_RETURN_POINTER(t);
}

/*
 * intset_union function
 * A || B takes the set union, and produces an intSet containing all the elements of A and B
 * That is, A ∪ B
*/
PG_FUNCTION_INFO_V1(intset_union);
Datum
    intset_union(PG_FUNCTION_ARGS)
{
    intSet *t1 = (intSet *)PG_GETARG_POINTER(0);
    intSet *t2 = (intSet *)PG_GETARG_POINTER(1);
    intSet *t;
    int sizeT1 = (t1->length) / size_16 - 1;
    int sizeT2 = (t2->length) / size_16 - 1;
    int i = 0, j = 0, k = 0;

    int *arr = malloc((sizeT1 + sizeT2) * sizeof(int));
    while (i < sizeT1 && j < sizeT2) // iterating over t1 and t2
    {
        if (t1->data[i] < t2->data[j])      // if data in t1 smaller than in t2
            arr[k++] = t1->data[i++];       // insert t1 data in the array and increament t1 iterator
        else if (t2->data[j] < t1->data[i]) // if data in t2 smaller than in t1
            arr[k++] = t2->data[j++];       // insert t2 data in the array and increament t2 iterator
        else                                // if data in t2 is equal to t1
        {
            arr[k++] = t1->data[i++]; // insert either in array and increament both iterators
            j++;
        }
    }
    while (i < sizeT1)
        arr[k++] = t1->data[i++]; // insert rest of the t1 in array
    while (j < sizeT2)
        arr[k++] = t2->data[j++]; // insert rest of the t2 in array

    t = (intSet *)palloc(VARHDRSZ + k * sizeof(int)); // macro VARHDRSZ to refer to the size of the overhead for a variable length type.
    SET_VARSIZE(t, VARHDRSZ + k * sizeof(int));
    memcpy(t->data, arr, k * sizeof(int));
    free(arr); // free temp array
    PG_RETURN_POINTER(t);
}

/*
 * intset_disjunction function
 * A !! B Takes the set disjunction, and produces an intSet containing elements that are in A and not in B,
 * or that are in B and not in A
*/
PG_FUNCTION_INFO_V1(intset_disjunction);
Datum
    intset_disjunction(PG_FUNCTION_ARGS)
{
    intSet *t1 = (intSet *)PG_GETARG_POINTER(0);
    intSet *t2 = (intSet *)PG_GETARG_POINTER(1);
    intSet *t;
    int sizeT1 = (t1->length) / size_16 - 1;
    int sizeT2 = (t2->length) / size_16 - 1;
    int i = 0, j = 0, k = 0;

    int *arr = malloc((sizeT1 + sizeT2) * sizeof(int));
    while (i < sizeT1 && j < sizeT2)
    {
        if (t1->data[i] < t2->data[j])      // if data in t1 smaller than in t2
            arr[k++] = t1->data[i++];       // insert t1 data in the array and increament t1
        else if (t2->data[j] < t1->data[i]) // if data in t2 smaller than in t1
            arr[k++] = t2->data[j++];       // insert t2 data in the array and increament t2
        else                                // if data in t2 is equal to t1
        {
            i++; // insert neither of them in the array and increament both
            j++;
        }
    }
    while (i < sizeT1)
        arr[k++] = t1->data[i++]; // insert rest of the t1 in array
    while (j < sizeT2)
        arr[k++] = t2->data[j++]; // insert rest of the t2 in array

    t = (intSet *)palloc(VARHDRSZ + k * sizeof(int)); // macro VARHDRSZ to refer to the size of the overhead for a variable length type.
    SET_VARSIZE(t, VARHDRSZ + k * sizeof(int));
    memcpy(t->data, arr, k * sizeof(int));
    free(arr);
    PG_RETURN_POINTER(t);
}

/*
 * intset_difference function
 * A - B Takes the set difference, and produces an intSet containing elements that are in A and not in B
*/
PG_FUNCTION_INFO_V1(intset_difference);
Datum
    intset_difference(PG_FUNCTION_ARGS)
{
    intSet *t1 = (intSet *)PG_GETARG_POINTER(0);
    intSet *t2 = (intSet *)PG_GETARG_POINTER(1);
    intSet *t;
    int sizeT1 = (t1->length) / size_16 - 1;
    int sizeT2 = (t2->length) / size_16 - 1;
    int i = 0, j = 0, k = 0;

    int *arr = malloc((sizeT1 + sizeT2) * sizeof(int));
    while (i < sizeT1 && j < sizeT2)
    {
        if (t1->data[i] < t2->data[j])      // if data t1 < data t2
            arr[k++] = t1->data[i++];       // insert in t1 in array and increment t1 iterator
        else if (t2->data[j] < t1->data[i]) // if data t2 < data t1
            j++;                            // increment t2 iterator
        else                                // if data t1 = data t2
        {
            i++; // increment both the iterators
            j++;
        }
    }
    while (i < sizeT1)
        arr[k++] = t1->data[i++]; // insert remaining from t1

    t = (intSet *)palloc(VARHDRSZ + k * sizeof(int)); // macro VARHDRSZ to refer to the size of the overhead for a variable length type.
    SET_VARSIZE(t, VARHDRSZ + k * sizeof(int));
    memcpy(t->data, arr, k * sizeof(int));
    free(arr); // free temp array
    PG_RETURN_POINTER(t);
}

/*
 * this function process the input string and returns true if valid and false otherwise
*/
int processString(node *n, char *str, int *size)
{
    int i;
    int isEmpty = false;
    int start = 0;
    int next_start = 0;
    int isLast = false;
    int end;
    int val;
    // char *input = malloc(strlen(str) * sizeof(char));
    char input[size_1024 * size_1024];

    intSet *t;
    int index = trim(str, input); // trimming leading and trailing spaces

    if (input[0] != '{' || input[index] != '}') //invalid braces at both the ends
        return false;
    if (index == 1) // empty intset
    {
        t = (intSet *)palloc(VARHDRSZ); // macro VARHDRSZ to refer to the size of the overhead for a variable length type.
        SET_VARSIZE(t, VARHDRSZ);
        PG_RETURN_POINTER(t);
    }

    while (next_start < index) // find next value to convert
    {
        start = next_start + 1;
        isLast = false;
        for (i = start; i < index; i++) // finding substring
        {
            if (input[i] == ',') // if the character is ','
            {
                next_start = i;
                isLast = true;
                break;
            }
        }

        if (!isLast) // checking is reached the end of the string
        {
            if (start == 1)
                isEmpty = true;
            next_start = index;
        }

        end = next_start - 1;
        while (end >= start && input[end] == ' ') // ignoring the spaces in between
            end--;

        if (end < start) // check for empty substring or invalid spaces
        {
            if (!isEmpty)
                return false;
            else
            {
                intSet *t = (intSet *)palloc(VARHDRSZ);
                SET_VARSIZE(t, VARHDRSZ);
                PG_RETURN_POINTER(t);
            }
        }
        if (processSubString(input, start, end, &val)) // processing the substring
            insert(n, val, &*size);                    // inserting the valid value
        else
            return false; // return false is the substring is not valid
    }
    // free(input); // free temp array
    return true;
}

/*
 * this function check for integer validity and returns true or false
*/
int isValidInteger(int *out, char *s)
{
    char *errstr;
    long val = strtol(s, &errstr, 10);
    if (val > INT32_MAX || val < 0 ||
        (errno == ERANGE && val == INT32_MAX)) // if the integer is out of range
        return false;                          // return false
    if (*errstr != '\0')                       // if errstr is not NULL
        return false;                          // returns false
    *out = (int)val;                           // type cast long to int
    return true;                               // return true, valid integer
}

/*
 * this function check for substring validity and returns true or false
*/
int processSubString(char *s, int start, int end, int *out)
{
    int r;
    char *temp = malloc((end - start + 2) * sizeof(char)); // create temp string dynamically
    memcpy(temp, &s[start], end - start + 1);              // copy
    temp[end - start + 1] = '\0';                          // terminating with NULL
    r = isValidInteger(out, temp);                         // check for integer validity
    free(temp);                                            // free temp space
    return r;
}

/*
 * this function trims trailing and leading white spaces and returns the last index of the string
*/
int trim(char *str, char *input)
{
    int index, i;
    index = 0;
    while (str[index] == ' ') // Trim leading white spaces
        index++;

    i = 0;
    while (str[i + index] != '\0') // Shift all trailing characters to its left
    {
        input[i] = str[i + index];
        i++;
    }
    input[i] = '\0'; // Terminate intermediate string with NULL

    i = 0;
    index = -1;
    while (input[i] != '\0') // Trim trailing white spaces
    {
        if (input[i] != ' ')
            index = i;
        i++;
    }
    input[index + 1] = '\0'; // Terminate final string with NULL
    return index;            // return the last char index
}

/*
 * this function creates hash value and returns the same
*/
int getHashValue(int val)
{
    if (val < 0)
        return val % size_1024 + size_1024;
    return val % size_1024;
}

/*
 * this function creates empty nodes and return the same
*/
node *createEmptyNode()
{
    node *n = malloc(size_1024 * sizeof(node));
    int i;
    for (i = 0; i < size_1024; i++)
    {
        n[i].value = 0;
        n[i].exist = false;
        n[i].next = NULL;
    }
    return n;
}

/*
 * this function checks if the value exists adn return the node
*/
node *find(node *n, int value)
{
    while (n->next) // traversing each node in the list
    {
        if (n->value == value) // if value exists
            return n;          // return node
        n = n->next;           // iterate to the next node
    }
    return n; // return the node
}

/*
 * this function inserts valid int value in the nodes
*/
node *insert(node *n, int val, int *size)
{
    int value = getHashValue(val);

    if (!n[value].exist) // if value exists
    {
        n[value].value = val;
        n[value].exist = true;
        *size = *size + 1;
    }

    else // if value does not exist
    {

        node *get_result = find(&n[value], val); // find the specific node
        if (get_result->value == val)

            return n; //return the node
        else
        {

            node *new_node = malloc(sizeof(node)); // create a new node
            new_node->value = val;                 // insert the value
            new_node->exist = true;                // mark exist
            new_node->next = NULL;                 // set it as the last node in the list
            get_result->next = new_node;           // next node
            *size = *size + 1;                     // increase the size of the linkedlist
        }
    }

    return n; //return the node
}

/*
 * this function converts into array of unique values
*/
int *convertIntoIntArr(node *n, int size)
{
    node *temp;
    node *last;
    int *arr = calloc(size, sizeof(int)); // creating dynamic array
    int i;
    int j = 0;
    for (i = 0; i < size_1024; i++) // loop over all the nodes
    {
        if (n[i].exist) // checking for duplicate values
        {
            arr[j++] = n[i].value;
            temp = n[i].next;
            while (temp)
            {
                arr[j++] = temp->value; // get value into the arr
                last = temp;
                temp = temp->next; // iterating over temp node
                free(last);        // free last node
            }
        }
    }
    free(n); // free temp node
    return arr;
}

/*
 * this function is the helper function for qsort
*/
int partition(int arr[], int low, int high)
{
    int key;
    key = arr[low];
    while (low < high)
    {
        while (low < high && arr[high] >= key)
            high--;
        if (low < high)
            arr[low++] = arr[high];
        while (low < high && arr[low] <= key)
            low++;
        if (low < high)
            arr[high--] = arr[low];
    }
    arr[low] = key;
    return low; // return the postion
}

/*
 * this function sorts the array arr using quick sort algorithm
*/
void qSort(int arr[], int start, int end)
{
    int pos;
    if (start < end)
    {
        pos = partition(arr, start, end); // get the postion
        qSort(arr, start, pos - 1);       // recurrsive call
        qSort(arr, pos + 1, end);         // recurrsive call
    }
    return;
}

/*
 * this function search an element in the array arr using binary search algorithm
*/
int binarySearch(int arr[], int left, int right, int x)
{
    if (right >= left) // searching on sorted array in asc order
    {
        int mid = left + (right - left) / 2;
        if (arr[mid] == x)
            return mid; // return the position of x
        else if (arr[mid] > x)
            return binarySearch(arr, left, mid - 1, x); //recurrsive call
        else if (arr[mid] < x)
            return binarySearch(arr, mid + 1, right, x); //recurrsive call
    }
    return -1; // return -1 if the value is not found
}