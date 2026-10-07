// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation header

// ========================================================================= //
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "../utils.h"

#include "IR.h"

// ========================================================================= //
void iaerr (char* msg)
{
        printf ("%s\n", msg);
        exit (EXIT_FAILURE);
}

IR* new_IR (char* name)
{
        IR* fun = malloc (sizeof (IR));
        if (!fun) {
                iaerr ("IR allocation failed at new_fun");
        }

        fun->name = name;
        fun->num_args = 0;
        fun->num_vars = 0;

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
        REQUIRES (index < fun->num_inst);
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
        // reuse an existing pool entry if we already have this constant
        uint16_t j = 0;
        while (j < I->num_int) {
                if (I->int_pool[j] == x) {
                        return j;
                }
                j++;
        }

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
// Printing and serialization

static const char* op_name (uint8_t op)
{
        switch (op) {
                case OP_NOP :          return "nop";
                case OP_BIPUSH :       return "bipush";
                case OP_ILDC :         return "ildc";
                case OP_ALDC :         return "aldc";
                case OP_VLOAD :        return "vload";
                case OP_PTLOAD :       return "ptload";
                case OP_DYLOAD :       return "dyload";
                case OP_VSTORE :       return "vstore";
                case OP_PTSTORE :      return "ptstore";
                case OP_DYSTORE :      return "dystore";
                case OP_POP :          return "pop";
                case OP_DUP :          return "dup";
                case OP_SWAP :         return "swap";
                case OP_IADD :         return "iadd";
                case OP_ISUB :         return "isub";
                case OP_IMUL :         return "imul";
                case OP_IDIV :         return "idiv";
                case OP_SLOAD :        return "sload";
                case OP_INEG :         return "ineg";
                case OP_IF :           return "if";
                case OP_ICMPEQ :       return "icmpeq";
                case OP_ICMPLT :       return "icmplt";
                case OP_ICMPGE :       return "icmpge";
                case OP_ICMPGT :       return "icmpgt";
                case OP_ICMPLE :       return "icmple";
                case OP_GOTO :         return "goto";
                case OP_RETURN :       return "return";
                case OP_INVOKESTATIC : return "invokestatic";
                default :              return NULL;
        }
}

// number of operand bytes following the opcode
static int op_operands (uint8_t op)
{
        switch (op) {
                case OP_BIPUSH :
                case OP_VLOAD :
                case OP_VSTORE :
                        return 1;
                case OP_ILDC :
                case OP_ALDC :
                case OP_IF :
                case OP_GOTO :
                case OP_INVOKESTATIC :
                        return 2;
                default :
                        return 0;
        }
}

static void print_IR (IR_Program* I, IR* fun, uint16_t index)
{
        printf ("\nfunction #%u '%s' (args: %u, vars: %u, %u bytes)\n",
                index, fun->name, fun->num_args, fun->num_vars, fun->num_inst);

        uint16_t pc = 0;
        while (pc < fun->num_inst) {
                uint8_t op = fun->inst[pc];
                const char* name = op_name (op);
                int n = op_operands (op);

                printf ("  %04u: ", pc);

                int b = 0;
                while (b <= 2) {
                        if (b <= n) {
                                printf ("%02X ", fun->inst[pc + b]);
                        } else {
                                printf ("   ");
                        }
                        b++;
                }

                if (!name) {
                        printf ("<unknown opcode>\n");
                        pc++;
                        continue;
                }
                printf (" %-13s", name);

                if (n == 1) {
                        uint8_t x = fun->inst[pc + 1];
                        if (op == OP_BIPUSH) {
                                printf ("%d", (int8_t) x);
                        } else {
                                printf ("V[%u]", x);
                        }
                } else if (n == 2) {
                        uint16_t x = (uint16_t) ((fun->inst[pc + 1] << 8) |
                                                  fun->inst[pc + 2]);
                        if (op == OP_IF || op == OP_GOTO) {
                                printf ("%+d  -> %04d", (int16_t) x,
                                        pc + (int16_t) x);
                        } else if (op == OP_ILDC) {
                                printf ("int_pool[%u] = %d", x, I->int_pool[x]);
                        } else if (op == OP_INVOKESTATIC) {
                                printf ("%s", x < I->num_fun ?
                                              I->fun_pool[x]->name : "?");
                        } else {
                                printf ("%u", x);
                        }
                }
                printf ("\n");

                pc += 1 + n;
        }
}

void print_IR_Program (IR_Program* I)
{
        printf ("\n===== nD bytecode =====\n");

        printf ("int pool (%u):", I->num_int);
        uint16_t i = 0;
        while (i < I->num_int) {
                printf (" [%u]=%d", i, I->int_pool[i]);
                i++;
        }
        printf ("\n");

        i = 0;
        while (i < I->num_fun) {
                print_IR (I, I->fun_pool[i], i);
                i++;
        }
        printf ("=======================\n");
}

static void write_u8 (FILE* out, uint8_t x)
{
        fputc (x, out);
}

static void write_u16 (FILE* out, uint16_t x)
{
        write_u8 (out, (uint8_t) (x >> 8));
        write_u8 (out, (uint8_t) x);
}

static void write_i32 (FILE* out, int32_t x)
{
        uint32_t u = (uint32_t) x;
        write_u16 (out, (uint16_t) (u >> 16));
        write_u16 (out, (uint16_t) u);
}

void write_IR_Program (IR_Program* I, FILE* out)
{
        fwrite ("nDBC", 1, 4, out);
        write_u16 (out, NDBC_VERSION);

        write_u16 (out, I->num_int);
        uint16_t i = 0;
        while (i < I->num_int) {
                write_i32 (out, I->int_pool[i++]);
        }

        write_u16 (out, I->num_str);
        i = 0;
        while (i < I->num_str) {
                size_t len = strlen (I->str_pool[i]);
                write_u16 (out, (uint16_t) len);
                fwrite (I->str_pool[i], 1, len, out);
                i++;
        }

        write_u16 (out, I->num_fun);
        i = 0;
        while (i < I->num_fun) {
                IR* fun = I->fun_pool[i++];
                write_u8 (out, fun->num_args);
                write_u8 (out, fun->num_vars);
                write_u16 (out, fun->num_inst);
                fwrite (fun->inst, 1, fun->num_inst, out);
        }
}

void IR_Program_free (IR_Program* I)
{
        uint16_t i = 0;
        while (i < I->num_fun) {
                free (I->fun_pool[i]->inst);
                free (I->fun_pool[i]);
                i++;
        }
        free (I->fun_pool);
        free (I->int_pool);
        free (I->str_pool);
        free (I);
}

// ========================================================================= //


// ========================================================================= //
// Scope stack

Vstack* new_vstack ()
{
        Vstack* S = malloc (sizeof (Vstack));
        if (!S) {
                iaerr ("Vstack allocation failed");
        }

        S->num = 0;
        S->cap = 4;
        S->stack = malloc (sizeof (Vscope) * S->cap);
        if (!S->stack) {
                iaerr ("Vstack scope list allocation failed");
        }

        S->top = 0;
        S->max = 0;

        return S;
}

void push_vstack (Vstack* S, Var_List* V)
{
        size_t n = V ? V->num_var : 0;
        if ((size_t) S->top + n > UINT8_MAX) {
                iaerr ("too many local variables in one function (max 255)");
        }

        S->stack[S->num].vars = V;
        S->stack[S->num].base = S->top;
        S->num++;

        S->top += (uint8_t) n;
        if (S->top > S->max) {
                S->max = S->top;
        }

        if (S->num == S->cap) {
                S->cap *= 2;
                Vscope* new = malloc (sizeof (Vscope) * S->cap);
                if (!new) {
                        iaerr ("Vstack scope list allocation failed");
                }
                size_t i = 0;
                while (i < S->num) {
                        new[i] = S->stack[i];
                        i++;
                }
                free (S->stack);
                S->stack = new;
        }
}

void pop_vstack (Vstack* S)
{
        REQUIRES (S->num > 0);
        S->num--;
        // slots of the exited scope are free for the next sibling scope
        S->top = S->stack[S->num].base;
}

int search_vstack (Vstack* S, char* name, Var** found)
{
        size_t i = S->num;
        while (i > 0) {
                i--;
                Var_List* V = S->stack[i].vars;
                if (V == NULL) {
                        continue;
                }

                size_t j = 0;
                while (j < V->num_var) {
                        if (strcmp (V->variables[j]->name, name) == 0) {
                                if (found) {
                                        *found = V->variables[j];
                                }
                                return S->stack[i].base + (int) j;
                        }
                        j++;
                }
        }

        return -1;
}

void vstack_free (Vstack* S)
{
        free (S->stack);
        free (S);
}

int search_fun (AST_Program* A, char* name)
{
        size_t i = 0;
        while (i < A->function_count) {
                Astn* node = A->functions[i];
                if (node != NULL && node->kind == NODE_FUN_DEC &&
                    strcmp (node->data.fun_dec->fun_name, name) == 0) {
                        return (int) i;
                }
                i++;
        }

        return -1;
}

// ========================================================================= //
