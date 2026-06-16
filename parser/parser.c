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
Type* pointer_type (Token** tlist, size_t i, Type* type)
{
        // i is at *
        Type* t = malloc (sizeof (Type));
        t->kind = POINTER;
        t->pointer = NULL;

        if (i > 0 && tlist[i-1] == TOK_STAR) {
                t->pointer = pointer_type (tlist, i - 1, NULL);
        }

        if (!type) {
                return tlist_handler (tlist, i - 1, t);
        } else if (type->kind == FUNCTION) {
                (type->data.tree)->input = t;
                return tlist_handler (tlist, i - 1, type);
        }
}

Type* fun_type_handler (Token** tlist, size_t i, Type* type)
{
        // i is at ->
        Type* t = malloc (sizeof (Type));
        t->kind = FUNCTION;
        t->data.tree = malloc (sizeof (Type_Tree));

        if (!type) {
                perror ("Type is typed in wrong buddy.\n");
                exit (EXIT_FAILURE);
        }

        (t->data.tree)->output = type;
        (t->data.tree)->intput = NULL;


        return tlist_handler (tlist, i - 1, t);
}

Type* prim_type (Token** tlist, size_t i, Type* type)
{
        // i is at the prim type
        Type* t = malloc (sizeof (Type));
        if (tlist[i] == TOK_INT_TYPE) {
                t->kind = VALUE;
                t->data.base = INT;
        } else if (tlist[i] == TOK_INT_MUT_TYPE) {
                t->kind = VALUE;
                t->data.base = INT_MUT;
        } else if (tlist[i] == TOK_CHAR_TYPE) {
                t->kind = VALUE;
                t->data.base = CHAR;
        } else if (tlist[i] == TOK_CHAR_MUT_TYPE) {
                t->kind = VALUE;
                t->data.base = CHAR_MUT;
        }

        if (!type) {
                type = t;
        } else if (type->kind == FUNCTION) {
                (type->data.tree)->intput = t;
        } else {
                perror ("Type is typed in wrong buddy.\n");
                exit (EXIT_FAILURE);
        }

        return tlist_handler (tlist, i - 1, type);
}

Type* tlist_handler (Token** tlist, size_t i, Type* type)
{
        // PEEP THE TAIL RECURSION!!!
        if (i < 0) return type;

        if (tlist[i] == TOK_STAR) {
                return pointer_type (tlist, i, type);
        } else if (tlist[i] == TOK_FN_TYPE) {
                return fun_type (tlist, i, type);
        } else {
                return prim_type (tlist, i, type);
        }
}

Type* type_handler (Stream* S) 
{
        size_t list_index = 0;
        size_t list_size = 2;
        Token** tlist = malloc (sizeof (list_size));

        while (true) {
                if (stream_curr(S)->type == TOK_LSBRACE) {
                        break;
                } else if (stream_curr(S)->type == TOK_LBRACE) {
                        break;
                } else {
                        tlist[list_index++] = stream_curr (S);
                }
                
                if (list_index == list_size) {
                        Token** new = malloc (sizeof (list_size * 2));
                        
                        int i = 0;
                        while (i < list_size) {
                                new[i] = tlist[i];
                                i++;
                        }

                        list_size = list_size * 2;
                        free (tlist);
                        tlist = new;
                }
        }

        // Token** is now full of tokens indicating the type of whatever
        // function or variable


        return tlist_handler (tlist, list_index, NULL);
}

// ========================================================================= //


// ========================================================================= //
// Modular code for handling function body, loop body, user defined scopes, 
// variable declarations, etc. 
Astn** body_handler (Stream* S);


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

                // No value until we call the function!
                variable->value = NULL;

                // Name of variable
                variable->name = (stream_curr(S))->lexeme;
                
                // Now looking for colon
                curr_type = (stream_next (S))->type;
                if (curr_type != TOK_COLON) {
                        perror ("Didn't declare type of parameter\n");
                        exit (EXIT_FAILURE);
                }

                // Now looking for type
                variable->type = type_handler (S);

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
                perror ("forgot right parenthesis on this bro");
                exit (EXIT_FAILURE);
        } 

        // now expecting colon, declaring the return type of the function.
        curr_type = (stream_next (S))->type;
        if (curr_type != TOK_COLON) {
                // Didn't have colon to declare return type.
                perror ("forgot to declare return type on this bro");
                exit (EXIT_FAILURE);
        }

        // TODO: Handle return types, which can be function types
        // now expecting a type
        (stream_next (S))->type;
        fun->ret_type = type_handler (S);

        // TODO:   pass onto handling the body
        fun->body = body_handler (S);

        return fun;
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
