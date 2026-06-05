// Andy Kwak 2026
// My parser implementation, top-down AST generator

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tokenizer_array/token.h"
#include "tokenizer_array/real_stream.h"
#include "tokenizer_array/tokenize.h"

#include "ast.h"

AST_Program* parse (Stream* S);

bool is_statement  (Stream* S);

AST_Program* statement_handler  (Stream* S);
AST_Program* expression_handler (Stream* S);

AST_Program* parse (Stream* S)
//@requires \length(T) == n;
//@ensures isAST(\result);
{
        AST_Program* program = malloc (sizeof (AST_Program));
        if (!program) {
                perror ("program dude wtf hello\n");
                exit (EXIT_FAILURE);
        }


        program->function_count = 0;
        program->capacity = 4;
        program->functions = malloc (sizeof (Astn*) * program->capacity);

        if (is_error(S)) {
                perror ("Somehow got an error token fucker\n");
                exit (EXIT_FAILURE);
        }
        else if (is_statement(S)) {
                return statement_handler (S);
        }
        else if (!is_statement(S)){
                return expression_handler (S);
        }
        else { 
                perror ("No matched AST Node!");
                exit (EXIT_FAILURE);
        }
}

bool is_statement (Stream* S)
{
        switch ((stream_curr(S))->type) {
                case TOK_IF :
                        return true;
                case TOK_ELSEIF :
                        return true;
                case TOK_ELSE :
                        return true;
                case TOK_WHILE :
                        return true;
                case TOK_LPAREN :
                        return true;
                case TOK_LBRACE :
                        return true;
                default : 
                        return false;
        }
}

