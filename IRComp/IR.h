// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation header

// ========================================================================= //
#ifndef IR_H
#define IR_H


#include <stdlib.h>
#include <stdint.h>

#include "../tokenizer/token.h"

#include "../utils.h"

#include "../parser/parser.h"
#include "../parser/ast.h"

// ========================================================================= //
// This is one function. The variable table resizes itself on new body 
// declarations and scope exit. 
typedef struct IR {
        
} IR;


// This is the grand collective of functions
typedef struct IR_Program {
        size_t num;
        size_t cap;
        IR** program;
} IR_Program;

// ========================================================================= //


#endif
