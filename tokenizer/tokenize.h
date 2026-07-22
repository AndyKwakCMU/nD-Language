// ========================================================================= //
// Andy Kwak 2026

// Lazily evaluated tokenizer

// ========================================================================= //


// ========================================================================= //
#ifndef TOKENIZE_H
#define TOKENIZE_H


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "token.h"
#include "tokenize.h"
#include "real_stream.h"

// ========================================================================= //


// ========================================================================= //
Token** tokenize (FILE* fptr, int num_words);
Token* next_token (FILE* fptr, dbug* D);

Stream* new_stream (FILE* T);
Token* stream_curr (Stream* S);
Token* stream_next (Stream* S);
Token* stream_peek (Stream* S);

bool is_stream_end (Stream* S);
void stream_free   (Stream* S);

// ========================================================================= //


#endif
