#include <stdio.h>
/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */

#include <cs50.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    // only search for positive values of needle
    if (value < 0)
    {
        return false;
    }
    else
    {
        // to hold ultimate found/not-found value
        int found = 0;

        // use recursive method to add up found (=1) and not-found (=0) values
        return search_recursive(value, values, n, found);
    }
}

int search_recursive(int value, int values[], int n, int found)
{
    if (values[n/2] == value)
    {
        // Found it! Found it!
        return found + 1;
    }
    // if we have no more array, and still haven't found it
    else if (n == 0)
    {
        return found + 0;
    }
    else
    {
        // initialize a new half-size array
        int ta[n/2];

        // see if we need to check the first half
        // if so, populate the half-size array
        if (value < values[n/2])
        {
            for (int i = 0; i < n/2; i++)
            {
                ta[i] = values[i];
            }
        }
        else
        {
            for (int i = 0; i < n/2; i++)
            {
                // if even numbered the original array size, start from middle PLUS one index
                // otherwise start from middle-index
                // this way the second half of the original array is the same size
                ta[i] = (n % 2 == 0 ? n == 2 ? values[n/2] : values[n/2 + 1] : values[n/2 + 1 + i]);
            }
        }

        // because we are here, that means we haven't yet found it - send in found=1
        found += search_recursive(value, ta, n/2, 0);
        // rewind and send whatever we have in $found
        return found;
    }
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    // find the max value from values[]
    int maxval = 0;
    for (int i = 0; i < n; i++)
    {
        maxval = values[i] > maxval ? values[i] : maxval;
    }

    // create and initialize the counting array
    int counting[maxval + 1];
    for (int i = 0; i <= maxval; i++)
    {
        counting[i] = 0;
    }

    // populate the counting array from values[]
    for (int i = 0; i < n; i++)
    {
        counting[values[i]]++;
    }

    // create a new array for the sorted values
    //int sorted[n];
    int sortedi = 0;
    for (int i = 0; i <= maxval; i++)
    {
        if (counting[i] > 0)
        {
            for (int j = 0; j < counting[i]; j++)
            {
                values[sortedi] = i;
                sortedi++;
            }
        }
    }

    return;
}
