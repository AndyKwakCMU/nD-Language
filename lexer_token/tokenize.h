#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "token.h"
#include "tokenize.h"


Token** tokenize (FILE* fptr, int num_words);
Token* next_token (FILE* fptr);

#endif