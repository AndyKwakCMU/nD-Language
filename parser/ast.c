

#include <stdio.h>
#include <stdlib.h>


#include "ast.h"

Astn* new_literal_astn (Node_Type type, Literal_Expr literal)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("FUCK YOU LITERAL");
                exit (EXIT_FAILURE);
        }

        A->type = type;
        A->data.literal = literal;
        return A;
}

Astn* new_binary_astn (Node_Type type, Binary_Expr binary)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("FUCK YOU BINARY");
                exit (EXIT_FAILURE);
        }

        A->type = type;
        A->data.binary = binary;
        return A;
}

Astn* new_urnary_astn (Node_Type type, Urnary_Expr urnary)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("FUCK YOU URNARY");
                exit (EXIT_FAILURE);
        }

        A->type = type;
        A->data.urnary = urnary;
        return A;
}