// ========================================================================= //
// Andy Kwak 2026

// My AST helper function for the AST object
// ========================================================================= //


// ========================================================================= //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

// ========================================================================= //


// ========================================================================= //
GUser_Types* new_GUser ()
{
        GUser_Types* G = malloc (sizeof (GUser_Types));
        G->index = 0;
        G->size = 4;
        G->typelist = malloc (sizeof(User_Type*) * 4);
        return G;
}

void GUser_add (GUser_Types* G, User_Type* U)
{
        G->typelist[(G->index)++] = U;

        if (G->index == G->size) {
                size_t cap = G->size * 2;
                User_Type** new = malloc (sizeof (User_Type*) * cap);

                size_t i = 0;
                size_t n = G->index;
                User_Type** rem = G->typelist;
                while (i < n) {
                        new[i] = rem[i];
                        i++;
                }

                free (rem);
                G->typelist = new;
                G->size = cap;
        }
}

bool in_GUser (GUser_Types* G, char* name)
{
        size_t i = 0;
        size_t n = G->index;
        while (i < n) {
                char* curr_name = G->typelist[i]->struct_name;
                if (strcmp (curr_name, name) == 0) {
                        return true;
                }
                i++;
        }
        return false;
}

User_Type* get_GUser (GUser_Types* G, char* name)
{
        size_t i = 0;
        size_t n = G->index;
        while (i < n) {
                char* curr_name = G->typelist[i]->struct_name;
                if (strcmp (curr_name, name) == 0) {
                        return G->typelist[i];
                }
                i++;
        }
        perror ("User defined type DNE\n");
        exit (EXIT_FAILURE);
}

// ========================================================================= //


// ========================================================================= //
void fun_param_add (Fun_Type* fun, Var* v)
{
        fun->params[(fun->num_param)++] = v;
        if (fun->num_param == fun->param_cap) {
                fun->param_cap = fun->param_cap * 2;
                Var** new = malloc (sizeof (Var*) * fun->param_cap);
                
                size_t i = 0;
                while (i < fun->num_param) {
                        new[i] = fun->params[i];
                        i++;
                }
                free (fun->params);
                fun->params = new;
        }
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
        char* name = fun->fun_name;
        if (strcmp(name, "main") == 0) {
                if (A->functions[0] == NULL) {
                        Astn* node = malloc (sizeof (Astn));
                        node->kind = NODE_FUN_DEC;
                        node->data.fun_dec = fun;
                        A->functions[0] = node;
                        printf ("Main function declared!!\n");
                } else {
                        perror ("more than one main function\n");
                        exit (EXIT_FAILURE);
                }
        } else {
                Astn* node = malloc (sizeof (Astn));
                node->kind = NODE_FUN_DEC;
                node->data.fun_dec = fun;
                A->functions[A->function_count] = node;
                A->function_count++;

                if (A->function_count == A->capacity) {
                        size_t new_cap = A->capacity * 2;
                        Astn** new = malloc (sizeof (Astn*) * new_cap);
                        
                        size_t i = 0;
                        size_t n = A->function_count;
                        while (i < n) {
                                new[i] = A->functions[i];
                                i--;
                        }
                        Astn** rem = A->functions;
                        A->functions = new;
                        A->capacity = new_cap;

                        free (rem);
                }
        }
}

// ========================================================================= //
