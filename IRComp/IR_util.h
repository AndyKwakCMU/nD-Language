// ========================================================================= //
// Andy Kwak 2026

// Disassembler for the IR bytecode -- for hand-verifying IRComp's output
// against IR.h's opcode reference, since there's no VM/JIT yet to run it.

// ========================================================================= //
#ifndef IR_UTIL_H
#define IR_UTIL_H

#include "IR.h"

// ========================================================================= //
void print_IR_Program (IR_Program* I);

void print_IR (IR_Program* I, IR* fun, size_t index);

// ========================================================================= //

#endif
