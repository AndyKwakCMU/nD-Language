#include <stdio.h>
#include "token.h"

void print_tokens (Token** T, int n);
void free_tokens (Token** T, int n);

void print_tokens (Token** T, int n)
{
        printf ("Printing Tokens...\n");
        int i = 0;
        
        while (i < n) {
                Token* t = T [i];
                printf ("Token #%d: %s  %s", i, t->type)
        }
}