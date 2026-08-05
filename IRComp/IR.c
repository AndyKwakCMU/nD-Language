// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation header

// ========================================================================= //
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "IR.h"

// ========================================================================= //
void iaerr (char* msg)
{
        printf ("%s\n", msg);
        exit (EXIT_FAILURE);
}

IR* new_IR ()
{
        IR* fun = malloc (sizeof (IR));
        if (!fun) {
                iaerr ("IR allocation failed at new_fun");
        }

        fun->num_inst = 0;
        fun->inst_cap = 4;
        fun->inst = malloc (sizeof (uint8_t) * fun->inst_cap);
        if (!fun->inst) {
                iaerr ("IR instruction list allocation failed");
        }

        return fun;
}

IR_Program* new_IR_Program ()
{
        IR_Program* I = malloc (sizeof (IR_Program));
        if (!I) {
                iaerr ("IR_Program allocation failed");
        }

        I->num_fun = 0;
        I->fun_cap = 4;
        I->fun_pool = malloc (sizeof (IR*) * I->fun_cap);
        if (!I->fun_pool) {
                iaerr ("IR_Program function pool allocation failed");
        }
        
        I->num_int = 0;
        I->int_cap = 4;
        I->int_pool = malloc (sizeof (int32_t) * I->int_cap);
        if (!I->int_pool) {
                iaerr ("IR_Program integer pool allocation failed");
        }

        I->num_str = 0;
        I->str_cap = 4;
        I->str_pool = malloc (sizeof (char*) * I->str_cap);
        if (!I->str_pool) {
                iaerr ("IR_Program string pool allocation failed");
        }

        return I;
}

void byte_add (IR* fun, uint8_t byte)
{
        fun->inst[fun->num_inst++] = byte;

        if (fun->num_inst== fun->inst_cap) {
                fun->inst_cap *= 2;
                uint8_t* new = malloc (sizeof (uint8_t) * fun->inst_cap);
                size_t i = 0;
                size_t n = fun->num_inst;

                while (i < n) {
                        new[i] = fun->inst[i];
                        i++;
                }
                free (fun->inst);
                fun->inst = new;
        }
}

uint16_t byte_index (IR* fun)
{
        return fun->num_inst;
}

void byte_add_index (IR* fun, uint16_t index, uint8_t byte)
{
        if (index >= fun->num_inst) {
                iaerr ("byte_add_index: index out of range of already-emitted bytecode");
        }
        fun->inst[index] = byte;
}

void IR_add_fun (IR_Program* I, IR* fun)
{
        I->fun_pool[I->num_fun++] = fun;

        if (I->num_fun == I->fun_cap) {
                I->fun_cap *= 2;
                IR** new = malloc (sizeof (IR*) * I->fun_cap);
                size_t i = 0;
                size_t n = I->num_fun;

                while (i < n) {
                        new[i] = I->fun_pool[i];
                        i++;
                }
                free (I->fun_pool);
                I->fun_pool = new;
        }
}

uint16_t IR_add_int (IR_Program* I, int32_t x)
{
        uint16_t ret = I->num_int;
        I->int_pool[I->num_int++] = x;

        if (I->num_int == I->int_cap) {
                I->int_cap *= 2;
                int32_t* new = malloc (sizeof (int32_t) * I->int_cap);
                size_t i = 0;
                size_t n = I->num_int;

                while (i < n) {
                        new[i] = I->int_pool[i];
                        i++;
                }
                free (I->int_pool);
                I->int_pool = new;
        }

        return ret;
}

uint16_t IR_add_str (IR_Program* I, char* s)
{
        uint16_t ret = I->num_str;
        I->str_pool[I->num_str++] = s;

        if (I->num_str == I->str_cap) {
                I->str_cap *= 2;
                char** new = malloc (sizeof (char*) * I->str_cap);
                size_t i = 0;
                size_t n = I->num_str;

                while (i < n) {
                        new[i] = I->str_pool[i];
                        i++;
                }
                free (I->str_pool);
                I->str_pool = new;
        }
        
        return ret;
}

// ========================================================================= //
Vstack* new_vstack ()
{
        Vstack* S = malloc (sizeof (Vstack));
        if (!S) {
                iaerr ("Vstack allocation failed");
        }

        S->num = 0;
        S->cap = 4;
        S->stack = malloc (sizeof (Var_List*) * S->cap);
        if (!S->stack) {
                iaerr ("Vstack scope array allocation failed");
        }

        return S;
}

void push_vstack (Vstack* S, Var_List* V)
{
        S->stack[S->num++] = V;

        if (S->num == S->cap) {
                S->cap *= 2;
                Var_List** new = malloc (sizeof (Var_List*) * S->cap);
                size_t i = 0;
                size_t n = S->num;

                while (i < n) {
                        new[i] = S->stack[i];
                        i++;
                }
                free (S->stack);
                S->stack = new;
        }
}

void pop_vstack (Vstack* S)
{
        if (S->num == 0) {
                iaerr ("pop_vstack: scope stack is already empty");
        }
        S->num--;
}

uint8_t search_vstack (Vstack* S, char* name)
{
        // Search innermost scope outward for correct lexical shadowing, but
        // compute the slot index as if scanning from the bottom (params
        // first), since that's the fixed numbering vload/vstore rely on.
        size_t i = S->num;
        while (i > 0) {
                i--;
                Var_List* V = S->stack[i];
                size_t j = 0;
                while (j < V->num_var) {
                        if (strcmp (V->variables[j]->name, name) == 0) {
                                uint16_t base = 0;
                                size_t k = 0;
                                while (k < i) {
                                        base += (uint16_t) S->stack[k]->num_var;
                                        k++;
                                }
                                return (uint8_t) (base + j);
                        }
                        j++;
                }
        }

        iaerr ("search_vstack: variable not found in any live scope");
        return 0; // unreachable
}

uint16_t search_fun (AST_Program* A, char* name)
{
        // IR_Comp appends to fun_pool in the same order it walks
        // A->functions, so a function's index there is also its fun_pool
        // index.
        size_t i = 0;
        size_t n = A->function_count;

        while (i < n) {
                Astn* ast = A->functions[i];
                if (ast != NULL && strcmp (ast->data.fun_dec->fun_name, name) == 0) {
                        return (uint16_t) i;
                }
                i++;
        }

        iaerr ("search_fun: function not found in program");
        return 0; // unreachable
}
// ========================================================================= //

