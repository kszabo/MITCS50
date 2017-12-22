#include <cs50.h>
#include <stdio.h>
#include <string.h>

int main(int argc, string argv[])
{
    // store key, make sure it is not larger than the alphabet
    int key;

    if (argc != 2)
    {
        printf("Usage: ./caesar k\n");
        return 1;
    }
    key = atoi(argv[1]) % 26;

    // get user input
    printf("plaintext: ");
    string pt = get_string();

    printf("ciphertext: ");
    for (int i = 0, n = strlen(pt); i < n; i++)
    {
        if (pt[i] >= 'a' && pt[i] <= 'z')
        {
            // if key makes the ascii too large, calculate where it would wrap around
            if ((int)(pt[i]) + key > 'z')
            {
                printf("%c", ('a' - 1) + (int)(pt[i]) + key - 'z');
            }
            else
            {
                // safely in the alphabet
                printf("%c", (int)(pt[i]) + key);
            }
        }
        else if (pt[i] >= 'A' && pt[i] <= 'Z')
        {
            // if key makes the ascii too large, calculate where it would wrap around
            if ((int)(pt[i]) + key > 'Z')
            {
                printf("%c", ('A' - 1) + (int)(pt[i]) + key - 'Z');
            }
            else
            {
                // safely in the alphabet
                printf("%c", (int)(pt[i]) + key);
            }
        }
        else
        {
            // not alphabet, no need to cypher
            printf("%c", pt[i]);
        }
    }

    printf("\n");
}