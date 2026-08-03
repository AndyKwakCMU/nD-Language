// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation header

// ========================================================================= //
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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

