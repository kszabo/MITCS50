#include <cs50.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    // get user input
    string name = get_string();

    // only check if user input is a string
    if (name != NULL)
    {
        // initialize the space char
        char p = ' ';

        // check each character
        for (int i = 0, n = strlen(name); i < n; i++)
        {
            // if previous character is a single space, then this is an initial
            if (p == ' ')
            {
                if (name[i] >= 'a' && name[i] <= 'z')
                {
                    //printf("%c", toupper(name[i])); // doesn't compile!!
                    printf("%c", name[i] - 32);
                }
                else
                {
                    printf("%c", name[i]);
                }
            }
            // save current character to check against
            p = name[i];
        }
        printf("\n");
    }

}