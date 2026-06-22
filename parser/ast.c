// ========================================================================= //
// Andy Kwak 2026

// My AST helper function for the AST object
// ========================================================================= //


// ========================================================================= //
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

// ========================================================================= //


// ========================================================================= //
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

Astn* new_fun_dec (Fun_Type* fun_dec)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("FUCK YOU function");
                exit (EXIT_FAILURE);
        }

        A->kind = NODE_FUN_DEC;
        A->data.fun_dec = fun_dec;
        return A;
}

Astn* new_fun_call (Fun_Call* fun_call)
{
        Astn* A = malloc (sizeof (Astn));
        if (!A) {
                perror ("Fuck you function call");
                exit (EXIT_FAILURE);
        }

        A->kind = NODE_FUN_CALL;
        A->data.fun_call = fun_call;
        return A;
}

// ========================================================================= //
AST_Program* new_Program () 
{
        AST_Program* A = malloc (sizeof (AST_Program));
        if (!A) {
                perror ("program dude wtf hello\n");
                exit (EXIT_FAILURE);
        }


        A->function_count = 1;
        A->capacity = 4;
        A->functions = malloc (sizeof (Astn*) * A->capacity);
        A->functions[0] = NULL;

        return A;
}

void program_add_fun (AST_Program* A, Fun_Type* fun)
{
        if (strcmp(fun->fun_name, "main") == 0) {
                if (A->functions[0] == NULL) {
                        A->functions[0] = fun;
                } else {
                        perror ("more than one main function\n");
                        exit (EXIT_FAILURE);
                }
        } else {
                A->functions[(A->function_count)++] = fun;

                if (A->function_count == A->capacity) {
                        size_t new_cap = A->capacity * 2;
                        Astn** new = malloc (sizeof (Astn*) * new_cap);
                        
                        size_t i = (A->function_count) - 1;
                        size_t n = A->function_count;
                        while (i > 0) {
                                new[i] = A->functions[i];
                                i--;
                        }
                        new[0] = A->functions[0];
                        Astn** rem = A->functions;
                        A->functions = new;
                        A->capacity = new_cap;

                        free (rem);
                }
        }
}