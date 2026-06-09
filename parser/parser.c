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


AST_Program* fun_handler (AST_Program* A, Stream* S)
{
        TokenType curr_type = (stream_next (S))->type;

        Fun_Type* fun = malloc (sizeof (Fun_Type));

        // expecting a function name
        // TODO : Add a redundant function name checker
        if (curr_type != TOK_IDENTIFIER) {
                perror ("yo you didn't write the function right\n");
                exit (EXIT_FAILURE);
        }
        
        char* fun_name = (stream_curr(S))->lexeme;
        

        // now expecting the left parenthesis
        curr_type = (stream_next (S))->type;

        if (curr_type != TOK_LPAREN) {
                perror ("yo what are you doing with this function\n");
                exit (EXIT_FAILURE);
        }

        // now expecting either a bunch of arguments or right parenthesis
        curr_type = (stream_next (S))->type;
        while (curr_type == TOK_IDENTIFIER) {
                
        }
}

AST_Program* parse (Stream* S)
//@requires \length(T) == n;
//@ensures isAST(\result);
{
        AST_Program* A = malloc (sizeof (AST_Program));
        if (!A) {
                perror ("program dude wtf hello\n");
                exit (EXIT_FAILURE);
        }


        A->function_count = 0;
        A->capacity = 4;
        A->functions = malloc (sizeof (Astn*) * A->capacity);

        if (is_error(S)) {
                perror ("Somehow got an error token fucker\n");
                exit (EXIT_FAILURE);
        }
        else if ((stream_curr(S))->type == TOK_FN) {
                // only fucking true case         
                return fun_handler (A, S);
        }
        // Later I need to implement like library stuff and stuff you feel me
        else { 
                perror ("No matched AST Node!");
                exit (EXIT_FAILURE);
        }
}

