// ========================================================================= //
// Andy Kwak 2026

// My parser implementation, top-down AST generator

// ========================================================================= //

// ========================================================================= //
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tokenizer/token.h"
#include "tokenizer/real_stream.h"
#include "tokenizer/tokenize.h"

#include "ast.h"

// ========================================================================= //

// ========================================================================= //
AST_Program* parse (Stream* S);


// ========================================================================= //


// ========================================================================= //
AST_Program* body_handler (AST_Program* A, Stream* S);
// Modular code for handling function body, loop body, user defined scopes, 
// variable declarations, etc. 

// ========================================================================= //


// ========================================================================= //
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
        int count = 0;
        while (curr_type == TOK_IDENTIFIER) {
                // Add new parameter variable
                Var* variable = malloc (sizeof (Var));
                Type* type = malloc (sizeof (Type));

                // No value until we call the function!
                variable->value = NULL;

                // Name of variable
                char* curr_lex = (stream_curr(S))->lexeme;
                
                // Now looking for colon
                curr_type = (stream_next (S))->type;
                if (curr_type != TOK_COLON) {
                        perror ("Didn't declare type of parameter\n");
                        exit (EXIT_FAILURE);
                }

                // Now looking for type
                curr_type = (stream_next (S))->type;
                if (curr_type == TOK_INT_TYPE) {
                        type->kind = VALUE;
                        (type->data).base = INT;
                } else if (curr_type == TOK_INT_MUT_TYPE) {
                        type->kind = VALUE;
                        (type->data).base = INT_MUT;
                } else if (curr_type == TOK_CHAR_TYPE) {
                        type->kind = VALUE;
                        (type->data).base = CHAR;
                } else if (curr_type == TOK_CHAR_MUT_TYPE) {
                        type->kind = VALUE;
                        (type->data).base = CHAR_MUT;
                } else if (curr_type == TOK_IDENTIFIER) {
                        // TODO: User defined types
                        prerror ("Missing implementation of user defined types!\n");
                        exit (EXIT_FAILURE);
                }

                // Either we are done or we have a comma indicating there is
                // another parameter to be read
                if ((stream_peek(S))->type == TOK_COMMA) {
                        curr_type = (stream_next (S))->type;
                }


                count++;
                curr_type = (stream_next (S))->type;
        }
        fun->num_param = count;

        if (curr_type != TOK_RPAREN) {
                // Didn't have right parenthesis after adding arguments.
        } 

        // now expecting colon, declaring the return type of the function.
        curr_type = (stream_next (S))->type;
        if (curr_type != TOK_COLON) {
                // Didn't have colon to declare return type.
        }

        // TODO: Handle return types, which can be function types
        // now expecting a type
        curr_type = (stream_next (S))->type;



        // TODO:   pass onto handling the body
}

// ========================================================================= //


// ========================================================================= //
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

// ========================================================================= //
