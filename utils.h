#ifndef UTILS_C
#define UTILS_C

#include <stdio.h>
#include <ctype.h>
#include "tokenizer/token.h"
#include "tokenizer/real_stream.h"

int word_counter (FILE* fcount);
char* tokenType2string (TokenType t);
void print_token (Token* tok);
void print_stream (Stream* S);

#endif