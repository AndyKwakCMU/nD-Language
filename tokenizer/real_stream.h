// ========================================================================= //
// Andy Kwak 2026

// Header file to distinguish between the stream implementation and the 
// array implementation of the tokenizer.

// ========================================================================= //


// ========================================================================= //
#ifndef REAL_STREAM_H
#define REAL_STREAM_H
#include "token.h"

struct dbug {
	size_t row;
	size_t col;
};

typedef struct dbug dbug;

typedef struct {
        FILE* fptr;
        Token* prev_token;
        Token* curr_token;
        Token* next_token;

        dbug* D;
} Stream;

// ========================================================================= //

#endif
