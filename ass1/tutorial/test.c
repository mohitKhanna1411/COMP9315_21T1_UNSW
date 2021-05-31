#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define true 1
#define false 0
#define MAX 1024 * 1024
// An efficient C++ program to remove all spaces
// from a string

// Function to remove all spaces from a given string

int trim(char *str, char *input)
{
    int index, i;

    /*
     * Trim leading white spaces
     */
    index = 0;
    while (str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }

    /* Shift all trailing characters to its left */
    i = 0;
    while (str[i + index] != '\0')
    {
        input[i] = str[i + index];
        i++;
    }
    input[i] = '\0'; // Terminate string with NULL

    /*
     * Trim trailing white spaces
     */
    i = 0;
    index = -1;
    while (input[i] != '\0')
    {
        if (input[i] != ' ' && input[i] != '\t' && input[i] != '\n')
            index = i;
        i++;
    }

    /* Mark the next character to last non white space character as NULL */
    input[index + 1] = '\0';
    return index;
}

int getInt(int *out, char *s)
{
    char *errstr;
    long val = strtol(s, &errstr, 10);

    if (val > INT32_MAX || val < 0)
        return false;
    if (*errstr != '\0')
        return false;
    *out = (int)val;
    return true;
}

int isValidString(char *s, int start, int end, int *out)
{
    int r;
    char *temp = malloc((end - start + 2) * sizeof(char));
    memcpy(temp, &s[start], end - start + 1);
    temp[end - start + 1] = '\0';
    r = getInt(out, temp);
    free(temp);
    return r;
}

int process(char *str)
{
    int i;
    int j = 0;
    int isEmpty = false;     /* check input string is empty or not */
    int start = 0;           /* the position start to convert to int */
    int next_start = 0;      /* next position to convert */
    int assign_flag = false; /* check reach the end of string */
    int end;                 /* the position end to convert to int */
    int val;                 /* convert val */
    char input[strlen(str)];
    // removeLeadingTrailingSpaces(str, input);

    int input_index = trim(str, input);

    printf("%s", input);

    if (input[0] != '{' || input[input_index] != '}')
        return false;
    /* string is empty */
    if (input_index + 1 == 2)
    {
        return true;
        // t = (intSet *)palloc(VARHDRSZ);
        // SET_VARSIZE(t, VARHDRSZ);
        // PG_RETURN_POINTER(t);
    }
    /* find the next converting position */
    while (next_start < input_index)
    // while (input[j++] != '\0')
    {
        start = next_start + 1;
        assign_flag = false;
        for (i = start; i < input_index; i++)
        {
            if (input[i] == ',')
            {
                next_start = i;
                assign_flag = true;
                break;
            }
        }
        /* check reach the end of string or not */
        if (!assign_flag)
        {
            if (start == 1)
                isEmpty = true;
            next_start = input_index;
        }
        /* ignore the spaces after integer */
        end = next_start - 1;
        while (end >= start && input[end] == ' ')
            end--;

        /* empty string or occur invalid space */
        if (end < start)
        {
            if (!isEmpty)
                return false;
            else
            {
                return true;
                // intSet *t = (intSet *)palloc(VARHDRSZ);
                // SET_VARSIZE(t, VARHDRSZ);
                // PG_RETURN_POINTER(t);
            }
        }
        /* check the string is valid or not */
        if (isValidString(input, start, end, &val))
            // insert(n, val, &*size);
            printf("%d", val);

        else
            return false;
    }
    return true;
}

// Driver program to test above function
int main()
{
    char *str = "   { 1 , 2     s        }    ";
    int flag = process(str);
    printf("\n[%d]", flag);
    return 0;
}
