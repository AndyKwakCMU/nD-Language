#ifndef UTILS_C
#define UTILS_C

#include <stdio.h>
#include <ctype.h>
#include "tokenizer/token.h"
#include "tokenizer/real_stream.h"


#define ASSERT(cond)   do { (void)(cond); } while (0)
#define REQUIRES(cond) do { (void)(cond); } while (0)
#define ENSURES(cond)  do { (void)(cond); } while (0)

#ifdef DEBUG
        #undef ASSERT
        #undef REQUIRES
        #undef ENSURES

        #define ASSERT(cond)   assert (cond)
        #define REQUIRES(cond) assert(cond)
        #define ENSURES(cond)  assert(cond)
#endif


int word_counter (FILE* fcount);
char* tokenType2string (TokenType t);
void print_token (Token* tok);
void print_stream (Stream* S);


void aerr (Token* t);
void serr (Token* t, char* msg);

#endif