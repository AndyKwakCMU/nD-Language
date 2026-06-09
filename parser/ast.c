

#include <stdio.h>
#include <stdlib.h>


#include "ast.h"

Astn* new_literal_astn (Literal_Expr* literal)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("FUCK YOU LITERAL");
                exit (EXIT_FAILURE);
        }

        A->kind = NODE_LITERAL;
        A->data.literal = literal;
        return A;
}

Astn* new_binary_astn (Binary_Expr* binary)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("FUCK YOU BINARY");
                exit (EXIT_FAILURE);
        }

        A->kind = NODE_BINARY_EXPR;
        A->data.binary = binary;
        return A;
}

Astn* new_urnary_astn (Urnary_Expr* urnary)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("FUCK YOU URNARY");
                exit (EXIT_FAILURE);
        }

        A->kind = NODE_UNARY_EXPR;
        A->data.urnary = urnary;
        return A;
}

Astn* new_fun_dec (Fun_Type* fun)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("FUCK YOU function");
                exit (EXIT_FAILURE);
        }

        A->kind = NODE_FUN;
        A->data.fun = fun;
        return A;
}