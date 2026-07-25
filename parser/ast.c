// ========================================================================= //
// Andy Kwak 2026

// My AST helper function for the AST object
// ========================================================================= //


// ========================================================================= //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "ast.h"
#include "../utils.h"

// ========================================================================= //


// ========================================================================= //
Var_List* new_varlist ()
{
        Var_List* L = malloc (sizeof (Var_List));
        if (!L) {
                perror ("variable list struct allocation failure\n");
                exit (EXIT_FAILURE);
        }

        L->num_var = 0;
        L->var_cap = 4;
        L->variables = malloc (sizeof (Var*) * L->var_cap);
        
        if (!L->variables) {
                perror ("variable list allocation failure\n");
                exit (EXIT_FAILURE);
        }


        return L;
}

void varlist_add (Var_List* L, Var* var)
{
        L->variables[L->num_var] = var;
        (L->num_var)++;

        if (L->num_var == L->var_cap) {
                size_t i = 0;
                size_t cap = L->var_cap * 2;
                Var** new = malloc (sizeof (Var*) * cap);

                while (i < cap) {
                        new[i] = L->variables[i];
                        i++;
                }

                free (L->variables);
                L->variables = new;
                L->var_cap = cap;
        }
}

bool isin_varlist (Var_List* V, char* name)
{
        if (varlist_get_var (V, name) != NULL) {
                return true;
        } else {
                return false;
        }
}

Var* varlist_get_var (Var_List* V, char* name)
{
        size_t i = 0;
        while (i < V->num_var) {
                Var* curr_var = V->variables[i];
                char* curr_name = V->variables[i]->name;

                if (strcmp(name, curr_name) == 0) {
                        return curr_var;
                }
                i++;
        }
        return NULL;
}

void varlist_free (Var_List* L); // TODO

// ========================================================================= //


// ========================================================================= //
GUser_Types* new_GUser ()
{
        GUser_Types* G = malloc (sizeof (GUser_Types));
        G->num = 0;
        G->cap = 4;
        G->typelist = malloc (sizeof(User_Type*) * 4);
        return G;
}

void GUser_add (GUser_Types* G, User_Type* U)
{
        G->typelist[(G->num)++] = U;

        if (G->num == G->cap) {
                size_t cap = G->cap * 2;
                User_Type** new = malloc (sizeof (User_Type*) * cap);

                size_t i = 0;
                size_t n = G->num;
                User_Type** rem = G->typelist;
                while (i < n) {
                        new[i] = rem[i];
                        i++;
                }

                free (rem);
                G->typelist = new;
                G->cap = cap;
        }
}

User_Type* get_GUser (GUser_Types* G, char* name)
{
        size_t i = 0;
        size_t n = G->num;
        while (i < n) {
                char* curr_name = G->typelist[i]->name;
                if (strcmp (curr_name, name) == 0) {
                        return G->typelist[i];
                }
                i++;
        }
        return NULL;
}

bool isin_GUser (GUser_Types* G, char* name)
{
        if (get_GUser (G, name) != NULL) {
                return true;
        }

        return false;
}

Var* get_GUser_var (GUser_Types* G, char* name)
{
        REQUIRES (G != NULL && name != NULL);
        size_t i = 0;
        size_t n = G->num;

        while (i < n) {
                User_Type* curr_type = G->typelist[i];
                ASSERT (curr_type != NULL);
                if (curr_type->kind == STRUCT) {
                        Var_List* V = curr_type->data.Struct;
                        ASSERT (V != NULL);
                        Var* var = varlist_get_var (V, name);
                        if (var != NULL) {
                                return var;
                        }
                }
                i++;
        }
        return NULL;
}

bool isin_GUser_var (GUser_Types* G, char* name)
{
        if (get_GUser_var (G, name) != NULL) {
                return true;
        }

        return false;
}

// ========================================================================= //


// ========================================================================= //
Fun_Type* new_fun (char* name)
{
        Fun_Type* F = malloc (sizeof (Fun_Type));
        if (!F) {
                perror ("function allocation failed\n");
                exit (EXIT_FAILURE);
        }

        F->fun_name = name;

        F->num_var = 1; // parameters are always index 0
        F->var_cap = 4;
        F->variables = malloc (sizeof (Var_List*) * F->var_cap);
        
        if (!F->variables) {
                perror ("function variable list allocation failed\n");
                exit (EXIT_FAILURE);
        }

        F->variables[0] = new_varlist ();

        return F;
}

bool isin_fun_varlist (Fun_Type* F, char* name)
{
        if (fun_varlist_get_var(F, name) != NULL) {
                return true;
        }

        return false;
}

Var* fun_varlist_get_var (Fun_Type* F, char* name)
{
        size_t i = 0;
        while (i < F->num_var) {
                Var_List* V = F->variables[i];
                size_t j = 0;
                while (V != NULL && j < V->num_var) {
                        if (strcmp(V->variables[j]->name, name) == 0) {
                                return V->variables[j];
                        }
                        j++;
                }
                i++;
        }

        return NULL;
}

void fun_param_add (Fun_Type* F, Var* v)
{
        varlist_add (F->variables[0], v);
}

void fun_var_dec_add (Fun_Type* F, Var_List* L)
{
        // L can be NULL
        F->variables[F->num_var] = L;
        F->num_var++;

        if (F->num_var == F->var_cap) {
                size_t i = 0;
                size_t n = F->num_var;
                size_t cap = F->var_cap * 2;
                Var_List** new = malloc (sizeof (Var_List*) * cap);
                if (!new) {
                        perror ("new fun variable list allocation failed\n");
                        exit (EXIT_FAILURE);
                }

                while (i < n) {
                        new[i] = F->variables[i];
                        i++;
                }

                free (F->variables);
                F->variables = new;
                F->var_cap = cap;
        }
}

Var_List* fun_var_rem (Fun_Type* F) 
{
        return F->variables[--(F->num_var)];
}

// ========================================================================= //


// ========================================================================= //
Fun_Call* new_call (char* name)
{
        Fun_Call* C = malloc (sizeof (Fun_Call));
        if (!C) {
                printf ("function call allocation fail\n");
                exit (EXIT_FAILURE);
        }

        C->fun_name = strdup (name);
        C->num_arg = 0;
        C->arg_cap = 4;

        C->args = malloc (sizeof (Astn*) * C->arg_cap);
        if (!C->args) {
                printf ("function call AST list allocation fail\n");
                exit (EXIT_FAILURE);
        }

        return C;
}

void call_add_arg (Fun_Call* C, Astn* arg)
{
        C->args[(C->num_arg)++] = arg;

        if (C->num_arg == C->arg_cap) {
                C->arg_cap *= 2;
                Astn** new = malloc (sizeof (Astn*) * C->arg_cap);
                size_t i = 0;
                while (i < C->num_arg) {
                        new[i] = C->args[i];
                        i++;
                }
                free (C->args);
                C->args = new;
        }
}

// ========================================================================= //


// ========================================================================= //
Body_Block* new_body ()
{
        Body_Block* B = malloc (sizeof (Body_Block));
        if (!B) {
                perror ("body allocation failure\n");
                exit (EXIT_FAILURE);
        }

        B->num_inst = 0;
        B->inst_cap = 4;
        B->inst = malloc (sizeof (Astn*) * B->inst_cap);
        if (!B->inst) {
                perror ("instructions list allocation error\n");
                exit (EXIT_FAILURE);
        }

        B->vars = NULL;

        return B;
}

void body_add_inst (Body_Block* B, Astn* inst)
{
        B->inst[(B->num_inst)++] = inst;

        if (B->num_inst == B->inst_cap) {
                B->inst_cap *= 2;
                Astn** new = malloc (sizeof (Astn*) * B->inst_cap);
                size_t i = 0;
                while (i < B->num_inst) {
                        new[i] = B->inst[i];
                        i++;
                }
                free (B->inst);
                B->inst = new;
        }
}

void body_add_var (Body_Block* B, Var* v)
{
        varlist_add (B->vars, v);
}

Var_List* body_get_varlist (Body_Block* B)
{
        return B->vars;
}

// ========================================================================= //


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
        if (!A->functions) {
                perror ("program function list allocation fail\n");
                exit (EXIT_FAILURE);
        }
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

bool isin_program_fun (AST_Program* A, char* name)
{
        if (program_get_fun (A, name) != NULL) {
                return true;
        }

        return false;
}

Fun_Type* program_get_fun (AST_Program* A, char* name)
{
        size_t i = 0;
        size_t n = A->function_count;

        if (n == 1 && A->functions[i] == NULL) {
                return NULL;
        }

        while (i < A->function_count) {
                Astn* node = A->functions[i];
                if (node != NULL) {
                        if (node->kind != NODE_FUN_DEC) {
                                printf ("node in programs list is not a function declaration\n");
                                exit (EXIT_FAILURE);
                        }

                        Fun_Type* F = node->data.fun_dec;
                        if (strcmp (F->fun_name, name) == 0) {
                                return F;
                        }
                }
                i++;
        }
        return NULL;
}

// ========================================================================= //
