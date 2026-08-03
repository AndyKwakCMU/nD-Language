// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation header

// ========================================================================= //
#ifndef IR_H
#define IR_H


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// ========================================================================= //
// This is one function. The variable table resizes itself on new body 
// declarations and scope exit. 
typedef struct IR {
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
//
// Stack Operations: 
//
// 0x57 pop
// 0x59 dup
// 0x5F swap
//
// Arithmetic Operations:
//
// 0x60 iadd
// 0x64 isub
// 0x68 imul
// 0x6C idiv
//
// Constants Operations:
//
// 0x10 bipush <b>              S -> S, x:w32 (Push byte into stack)
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
//
// 0x00 nop
// 0x9F if <o1, o2>             S, x -> S (pc=pc+(o1<<8|o2) if v != 0)
// 0xA7 goto <o1, o2>           S -> S    (pc=pc+(o1<<8|o2))
//
// Functions:
// 0xB8 invokestatic <c1, c2>   S, v1, ..., vn -> S, v 
//                              (fun_pool[c1<<8|c2] => g, g(v1,...,vn) = v)
// 0xB0 return                  return to caller
// 
// Memory:
// load address and store address 
//
// 0x2E ptload                  S, a:*      -> S, b   (b is a piece of function allocated memory)
// 0x4E ptstore                 S, a:*, b   -> S      (*a = b)
// 0x2F dyload                  S, a:*      -> S, b:* (b is a piece of dynamic pool memory)
// 0x4F dystore                 S, a:*, b:* -> S      (*a = b)
//
// 
// ========================================================================= //
IR* new_IR ();

void byte_add (IR* fun, uint8_t byte);

// 
IR_Program* new_IR_Program ();

void IR_add_fun (IR_Program* I, IR* fun);


// returns index of where the literal was added to
uint16_t IR_add_int (IR_Program* I, int32_t x);

uint16_t IR_add_str (IR_Program* I, char* s);

// ========================================================================= //


#endif

