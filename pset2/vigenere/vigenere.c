#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, string argv[])
{
    // get key from command line
    if (argc != 2)
    {
        printf("Usage: ./vigenere k\n");
        return 1;
    }

    string key = argv[1];
    for (int i = 0, n = strlen(key); i < n; i++)
    {
        if (isalpha(key[i]) == 0 )
        {
            printf("Usage: ./vigenere k\n");
            return 1;
        }
    }

    int keylength = strlen(key);

    // create the array of the key's ascii vals
    char asciikey[keylength];
    for (int i = 0; i < keylength; i++)
    {
        asciikey[i] = (int)tolower(key[i]) - 'a';

        //printf("asciikey[%i]=%i\n", i, asciikey[i]);
    }

    // get plaintext
    printf("plaintext: ");
    string pt = get_string();

    // variable to keep track of the alpha caracter
    int alphaplace = -1;
    // encipher
    printf("ciphertext: ");
    for (int i = 0, n = strlen(pt); i < n; i++)
    {
        if (isalpha(pt[i]))
        {
            alphaplace += 1;
            //printf("pt[%i]=%c; (int)\'%c\'=%i, mod of %i=%i, key[mod of %i]=%c, %i ----- ",
            //i, pt[i], pt[i], (int)pt[i], alphaplace, alphaplace % keylength, alphaplace % keylength, key[alphaplace % keylength], (int)asciikey[alphaplace % keylength]);

            if (pt[i] >= 'a' && pt[i] <= 'z')
            {
                if ((int)pt[i] + asciikey[alphaplace % keylength] > (int)'z')
                {
                    printf("%c", ('a' - 1) + (int)pt[i] + asciikey[alphaplace % keylength] - 'z');
                }
                else
                {
                    printf("%c", (int)pt[i] + asciikey[alphaplace % keylength]);
                }
            }
            else if (pt[i] >= 'A' && pt[i] <= 'Z')
            {
                if ((int)pt[i] + asciikey[alphaplace % keylength] > (int)'Z')
                {
                    printf("%c", ('A' - 1) + (int)pt[i] + asciikey[alphaplace % keylength] - 'Z');
                }
                else
                {
                    printf("%c", (int)pt[i] + asciikey[alphaplace % keylength]);
                }
            }
        }
        else
        {
            printf("%c", pt[i]);
        }
    }

    printf("\n");

    // print ciphertext
}