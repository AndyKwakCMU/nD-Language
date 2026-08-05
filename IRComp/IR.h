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
// 0x57 pop                     S, v -> S  (discard the unused result of a
//                                           statement-level expression)
// 0x59 dup                     S, v -> S, v, v
// 0x5F swap                    S, a, b -> S, b, a
//
// Arithmetic Operations:
//
// 0x60 iadd                    S, a, b -> S, x       (x = a + b)
// 0x64 isub                    S, a, b -> S, x       (x = a - b)
// 0x68 imul                    S, a, b -> S, x       (x = a * b)
// 0x6C idiv                    S, a, b -> S, x       (x = a / b)
// 0x6F ineg                    S, a    -> S, x       (x = -a)
//
// Comparison Operations (push 1 for true, 0 for false):
//
// 0x61 icmpeq                  S, a, b -> S, x       (x = a == b)
// 0x62 icmplt                  S, a, b -> S, x       (x = a < b)
// 0x63 icmpleq                 S, a, b -> S, x       (x = a <= b)
// 0x65 icmpgt                  S, a, b -> S, x       (x = a > b)
// 0x66 icmpgeq                 S, a, b -> S, x       (x = a >= b)
//
// Note: && and || are compiled with short-circuit evaluation using if/goto
// directly (see binary_comp in IRComp.c), not with dedicated opcodes.
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
// 0x9F if <o1, o2>             S, x -> S (pc=pc+(o1<<8|o2) if x == 0)
// 0xA7 goto <o1, o2>           S -> S    (pc=pc+(o1<<8|o2))
//
// The offset (o1<<8|o2) is a signed 16-bit two's complement value, added to
// pc, where pc is the address of the if/goto opcode byte itself (i.e.
// offset=3 jumps to the byte immediately after this 3-byte instruction).
//
// Functions:
// 0xB8 invokestatic <c1, c2>   S, v1, ..., vn -> S, v
//                              (fun_pool[c1<<8|c2] => g, g(v1,...,vn) = v)
// 0xB0 return                  S, v -> .  (return v to caller)
//
// Memory:
// load address and store address
// NOTE: not lowered by IRComp yet (structs/pointers/new are out of scope
// for the current core-language pass); documented for the future.
//
// 0x2E ptload                  S, a:*      -> S, b   (b is a piece of function allocated memory)
// 0x4E ptstore                 S, a:*, b   -> S      (*a = b)
// 0x2F dyload                  S, a:*      -> S, b:* (b is a piece of dynamic pool memory)
// 0x4F dystore                 S, a:*, b:* -> S      (*a = b)
//
// 0x6E sload                   S, a, b -> c          (c = a.b, struct access)
// 
//
// 
// ========================================================================= //
IR* new_IR ();

void byte_add (IR* fun, uint8_t byte);

// Index (within fun->inst) that the *next* byte_add call will write to.
// Used to record jump-instruction start addresses for later backpatching.
uint16_t byte_index (IR* fun);

// Overwrite an already-emitted byte at `index` (must be < fun->num_inst).
// Used to backpatch the two operand bytes of a forward if/goto once its
// jump target is known.
void byte_add_index (IR* fun, uint16_t index, uint8_t byte);


//
IR_Program* new_IR_Program ();

void IR_add_fun (IR_Program* I, IR* fun);


// returns index of where the literal was added to
uint16_t IR_add_int (IR_Program* I, int32_t x);

uint16_t IR_add_str (IR_Program* I, char* s);

// A stack of scopes (Var_List*) live during compilation of one function.
// Slot indices for vload/vstore are assigned by summing the sizes of all
// var-lists below a variable's owning scope, plus its position within that
// scope: params (always pushed first, at the bottom) occupy slots
// [0, num_args), and each pushed body scope's variables continue from
// wherever the scope below it left off. Sibling scopes (e.g. an if-body and
// its else-body) are never simultaneously on the stack, so they naturally
// reuse the same slot range.
typedef struct Varlist_Stack {
        size_t num;
        size_t cap;
        Var_List** stack;
} Vstack;

Vstack* new_vstack ();

void push_vstack (Vstack* S, Var_List* V);

void pop_vstack (Vstack* S);

// Errors out (iaerr) if the name isn't found in any scope on the stack.
uint8_t search_vstack (Vstack* S, char* name);

// Index into AST_Program->functions (which IR_Comp populates fun_pool in
// lockstep with), used as the invokestatic operand. Errors out if not found.
uint16_t search_fun (AST_Program* A, char* name);

// ========================================================================= //


#endif

