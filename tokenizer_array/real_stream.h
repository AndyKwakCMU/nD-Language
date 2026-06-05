
#include <stdio.h>
#include "token.h"

typedef struct {
        FILE* fptr;
        Token* prev_token;
        Token* curr_token;
} Stream;
