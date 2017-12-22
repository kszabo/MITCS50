#include <cs50.h>
#include <stdio.h>

int main(void)
{
    printf("give me an int: ");
    int i = get_int();
    printf("hello, %i\n", i);
}