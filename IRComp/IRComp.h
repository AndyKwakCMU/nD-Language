// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation compiler header
// AST_Program* -> IR_Program*

// ========================================================================= //
#ifndef IRCOMP_H
#define IRCOMP_H


#include <stdlib.h>
#include <stdint.h>


#include "../tokenizer/token.h"

#include "../utils.h"

#include "../parser/parser.h"
#include "../parser/ast.h"

#include "IR.h"

// ========================================================================= //
IR_Program* IR_Comp (AST_Program* A);


// ========================================================================= //

#endif