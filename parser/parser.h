// ========================================================================= //
// Andy Kwak 2026

// My parser implementation, top-down AST generator

// ========================================================================= //
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../tokenizer/token.h"
#include "../tokenizer/real_stream.h"
#include "../tokenizer/tokenize.h"

#include "ast.h"

// ========================================================================= //
AST_Program* parse (Stream* S);

// ========================================================================= //


#endif
