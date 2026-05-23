#ifndef TOKENIZE_H
#define TOKENIZE_H

Token** tokenize (FILE* fptr, int num_words);
Token* next_token (FILE* fptr);

#endif