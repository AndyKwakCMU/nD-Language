// ========================================================================= //
// Andy Kwak 2026

// Debugging utility function library for my tokenizer
// Made for the array implementation of the tokenizer

// ========================================================================= //


// ========================================================================= //
#ifndef TOKEN_UTILS_H
#define TOKEN_UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

// ========================================================================= //


// ========================================================================= //
void print_tokens (Token** T, int n);

void free_tokens (Token** T, int n);

// ========================================================================= //

#endif
