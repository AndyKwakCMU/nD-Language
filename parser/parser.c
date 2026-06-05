// Andy Kwak 2026
// My parser implementation, top-down AST generator

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tokenizer_array/token.h"
#include "tokenizer_array/fake_stream.h"
#include "tokenizer_array/tokenize.h"

#include "ast.h"


AST_Program* parse (Token** T, int n)
//@requires \length(T) == n;
//@ensures isAST(\result);
{
        AST_Program* program = malloc (sizeof (AST_Program));
        program->function_count = 0;
        program->capacity = 4;
        program->functions = malloc (sizeof (Astn*) * program->capacity);



        int i = 1;

        while (true) {
                if      (is_literal(T[i])) {
                        literal_handler   (T[i]);
                }
                else if (is_binary(T[i])) {
                        binary_handler    (T[i]);
                }
                else if (is_unary(T[i])) {
                        unary_handler     (T[i]);
                }
                else if (is_func_call(T[i])) {
                        func_call_handler (T[i]);
                }
                else { 
                        perror ("No matched AST Node!");
                        exit (EXIT_FAILURE);
                }
        }
}