/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"

int dictionaryWordCount;
int alphabetSize = 27;

typedef struct node
{
    bool isWord;
    struct node *children[27];
}
node;

// create the top node
node *dictionaryRoot;

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char *word)
{
    //fprintf(stdout, "word: %s\n", word);

    if (dictionaryWordCount == 0)
    {
        fprintf(stdout, "Dictionary is empty!\n");
        return 0;
    }

    // create a cursor and point it to root
    node *cursor = dictionaryRoot;

    int letterIndex = 0;
    for (int i = 0; i < strlen(word); i++)
    {
        if (tolower(word[i]) == '\'')
            letterIndex = 26;
        else
            letterIndex = tolower(word[i]) - 'a';

        //fprintf(stdout, "alphabet[%i]: %c, cursor->children[%i]\n", i, alphabet[letterIndex], letterIndex);

        if (cursor -> children[letterIndex] == NULL)
            return false;
        else
            cursor = cursor -> children[letterIndex];
    }

    // check if the end flag is set
    return cursor -> isWord;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    // open dictionary file
    FILE *dp = fopen(dictionary, "r");
    if (dp == NULL)
    {
        printf("Could not open %s.\n", dictionary);
        unload();
        return 1;
    }

    dictionaryRoot = malloc(sizeof(node));
    if (dictionaryRoot == NULL)
    {
        fprintf(stdout, "Could not allocate memory!\n");
        fclose(dp);
        unload();
        return 1;
    }

    // initialize root
    for (int a = 0; a < alphabetSize; a++)
    {
        dictionaryRoot -> children[a] = NULL;
    }
    dictionaryRoot -> isWord = false;

    // create a cursor and point it to root
    node *cursor = dictionaryRoot;

    int letterIndex  = 0;
    for (int c = fgetc(dp); c != EOF; c = fgetc(dp))
    {
        if (c != '\n')
        {
            //fprintf(stdout, "%c: alphabet[%i], cursor->children[%i]: %s\n", c, letterIndex, letterIndex, (char *)(cursor -> children[letterIndex]));

            // get index of character in alphabet array
            if (tolower(c) == '\'')
                letterIndex = 26;
            else
                letterIndex = tolower(c) - 'a';

            // if not yet in trie
            if (cursor -> children[letterIndex] == NULL)
            {
                // create a new node and initialize
                node *thisNode = malloc(sizeof(node));
                if (thisNode == NULL)
                {
                    fprintf(stdout, "Could not allocate memory!\n");
                    fclose(dp);
                    unload();
                    return 1;
                }

                for (int j = 0; j < alphabetSize; j++)
                {
                    thisNode -> children[j] = NULL;
                }
                thisNode -> isWord = false;

                // move cursor down the tree
                cursor -> children[letterIndex] = thisNode;
            }

            // letter is in trie (already or just added now), move the cursor only
            cursor = cursor -> children[letterIndex];
        }
        else
        {
            // mark last character with end-of-word
            cursor -> isWord = true;
            // keep the count
            dictionaryWordCount++;
            // reset the cursor
            cursor = dictionaryRoot;
        }

        //fprintf(stdout, "cursor -> children[%i] = %c\n", letterIndex, alphabet[letterIndex]);
    }

    fclose(dp);
    return 1;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return dictionaryWordCount;
}

void freeDictionary(node * cursor)
{
    // check each child
    for (int i = 0; i < alphabetSize; i++)
    {
        if (cursor -> children[i] != NULL)
        {
            freeDictionary(cursor -> children[i]);
        }
    }

    // checked all children, looks like all are NULL
    free(cursor);
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    node *cursor = dictionaryRoot;

    freeDictionary(cursor);

    return true;
}
