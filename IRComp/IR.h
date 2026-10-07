// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation header

// ========================================================================= //
#ifndef IR_H
#define IR_H


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../parser/ast.h"

// ========================================================================= //
// This is one function. Local variable slots are reused once a body's
// scope exits, so num_vars is the deepest the variable table ever gets.
typedef struct IR {
        char*    name;

        uint8_t  num_args;
        uint8_t  num_vars;

        uint8_t* inst;
        uint16_t num_inst;
        uint16_t inst_cap;
} IR;


// This is the grand collective of functions
typedef struct IR_Program {
        IR**     fun_pool;
        uint16_t num_fun;
        uint16_t fun_cap;

        int32_t* int_pool; // int32_t int32_t int 32_t!!!!!!!!!!!!!!!!!!!!!
        uint16_t num_int;
        uint16_t int_cap;

        char** str_pool;
        uint16_t num_str;
        uint16_t str_cap;
} IR_Program;

// ========================================================================= //
// Byte Code Reference
//
// This bytecode copies many parts of the CMU C0VM instructions.
// Every value on the operand stack is a 32 bit word. Multi-byte operands are
// big endian.
//
// Stack Operations:
//
// 0x57 pop                     S, v -> S
// 0x59 dup                     S, v -> S, v, v
// 0x5F swap                    S, v1, v2 -> S, v2, v1
//
// Arithmetic Operations:
//
// 0x60 iadd                    S, x, y -> S, x+y
// 0x64 isub                    S, x, y -> S, x-y
// 0x68 imul                    S, x, y -> S, x*y
// 0x6C idiv                    S, x, y -> S, x/y
//
// 0x6F ineg                    S, x -> S, -x
//
// Comparison Operations (push 1 if true, 0 if false):
//
// 0xA0 icmpeq                  S, x, y -> S, (x == y)
// 0xA1 icmplt                  S, x, y -> S, (x <  y)
// 0xA2 icmpge                  S, x, y -> S, (x >= y)
// 0xA3 icmpgt                  S, x, y -> S, (x >  y)
// 0xA4 icmple                  S, x, y -> S, (x <= y)
//
// Constants Operations:
//
// 0x10 bipush <b>              S -> S, x:w32 (Push sign extended byte into stack)
// 0x13 ildc <c1, c2>           S -> S, x:w32 (Access int pool index with two
//                                             bytes to create 16bit integer
//                                             int_pool[(c1 << 8) | c2])
//
// 0x14 aldc <c1, c2>           S -> S, a:*  (a = &string_pool[(c1 << 8) | c2])
//
//
// Local Variables:
//
// 0x15 vload <i>               S -> S, v (v = V[i])
// 0x36 vstore <i>              S, v -> S (V[i] = v)
//
// Control Flow:
// Jump offsets are signed 16 bit, relative to the address of the jump's
// own opcode.
//
// 0x00 nop
// 0x9F if <o1, o2>             S, x -> S (pc=pc+(int16_t)(o1<<8|o2) if x == 0)
// 0xA7 goto <o1, o2>           S -> S    (pc=pc+(int16_t)(o1<<8|o2))
//
// Functions:
// Every function returns exactly one value; functions returning none
// return 0. Arguments v1..vn land in V[0]..V[n-1] of the callee.
//
// 0xB8 invokestatic <c1, c2>   S, v1, ..., vn -> S, v
//                              (fun_pool[c1<<8|c2] => g, g(v1,...,vn) = v)
// 0xB0 return                  S, v -> . (return v to caller)
//
// Memory (not emitted yet):
// load address and store address
//
// 0x2E ptload                  S, a:*      -> S, b   (b is a piece of function allocated memory)
// 0x4E ptstore                 S, a:*, b   -> S      (*a = b)
// 0x2F dyload                  S, a:*      -> S, b:* (b is a piece of dynamic pool memory)
// 0x4F dystore                 S, a:*, b:* -> S      (*a = b)
//
// 0x6E sload                   S, a, b -> c          (c = a.b, struct access)
//
//
// Binary file format (.ndbc), all integers big endian:
//
// magic        "nDBC"
// u16          version (1)
// u16          int pool count, then each entry as i32
// u16          string pool count, then each entry as u16 length + bytes
// u16          function count, then for each function:
//                      u8 num_args, u8 num_vars, u16 code length, code bytes
//
// Function 0 is always main, the program entry point.
// ========================================================================= //
#define OP_NOP          0x00
#define OP_BIPUSH       0x10
#define OP_ILDC         0x13
#define OP_ALDC         0x14
#define OP_VLOAD        0x15
#define OP_PTLOAD       0x2E
#define OP_DYLOAD       0x2F
#define OP_VSTORE       0x36
#define OP_PTSTORE      0x4E
#define OP_DYSTORE      0x4F
#define OP_POP          0x57
#define OP_DUP          0x59
#define OP_SWAP         0x5F
#define OP_IADD         0x60
#define OP_ISUB         0x64
#define OP_IMUL         0x68
#define OP_IDIV         0x6C
#define OP_SLOAD        0x6E
#define OP_INEG         0x6F
#define OP_IF           0x9F
#define OP_ICMPEQ       0xA0
#define OP_ICMPLT       0xA1
#define OP_ICMPGE       0xA2
#define OP_ICMPGT       0xA3
#define OP_ICMPLE       0xA4
#define OP_GOTO         0xA7
#define OP_RETURN       0xB0
#define OP_INVOKESTATIC 0xB8

#define NDBC_VERSION    1

// ========================================================================= //
IR* new_IR (char* name);

void byte_add (IR* fun, uint8_t byte);

uint16_t byte_index (IR* fun);

void byte_add_index (IR* fun, uint16_t index, uint8_t byte);


//
IR_Program* new_IR_Program ();

void IR_add_fun (IR_Program* I, IR* fun);


// returns index of where the literal was added to
uint16_t IR_add_int (IR_Program* I, int32_t x);

uint16_t IR_add_str (IR_Program* I, char* s);

void print_IR_Program (IR_Program* I);

void write_IR_Program (IR_Program* I, FILE* out);

void IR_Program_free (IR_Program* I);

// ========================================================================= //
// Scope stack used while lowering. Each scope is a body's Var_List, whose
// variables occupy slots [base, base + num_var) of the function's
// variable table.
typedef struct Vscope {
        Var_List* vars;
        uint8_t   base;
} Vscope;

typedef struct Vstack {
        Vscope*  stack;
        size_t   num;
        size_t   cap;

        uint8_t  top;     // next free slot
        uint8_t  max;     // deepest the slots have gone
} Vstack;

Vstack* new_vstack ();

void push_vstack (Vstack* S, Var_List* V);

void pop_vstack (Vstack* S);

// Searches innermost scope first, returns -1 if name is not in scope
int search_vstack (Vstack* S, char* name, Var** found);

void vstack_free (Vstack* S);

// returns -1 if no function has that name
int search_fun (AST_Program* A, char* name);

// ========================================================================= //


#endif
