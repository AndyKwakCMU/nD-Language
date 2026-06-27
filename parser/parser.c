// ========================================================================= //
// Andy Kwak 2026

// My parser implementation, top-down AST generator

// ========================================================================= //

// ========================================================================= //
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../tokenizer/token.h"
#include "../tokenizer/real_stream.h"
#include "../tokenizer/tokenize.h"

#include "../utils.h"

#include "parser.h"
#include "ast.h"

// ========================================================================= //

// ========================================================================= //
AST_Program* parse (Stream* S);




// ========================================================================= //


// ========================================================================= //
int type_nud_lookup (GUser_Types* G, Token* tok)
{
        TokenType tok_type = tok->type;
        switch (tok_type) {
                case TOK_INT_TYPE :
                        return 0;
                        break;
                case TOK_INT_MUT_TYPE :
                        return 0;
                        break;
                case TOK_CHAR_TYPE :
                        return 0;
                        break;
                case TOK_CHAR_MUT_TYPE :
                        return 0;
                        break;
                case TOK_IDENTIFIER :
                        if (in_GUser (G, tok->lexeme)) {
                                return 0;
                        } else {
                                perror ("Undeclared type used");
                                exit (EXIT_FAILURE);
                        }
                        break;
                default :
                        perror ("Typing syntax error");
                        exit (EXIT_FAILURE);
                        break;
        }
}

int type_led_lookup (GUser_Types* G, Token* tok)
{
        TokenType tok_type = tok->type;
        switch (tok_type) {
                case TOK_ARROW_TYPE :
                        return 100;
                        break;
                case TOK_STAR :
                        return 200;
                        break;
                case TOK_RPAREN :
                        return -1;
                        break;
                case TOK_COMMA :
                        return -1;
                        break;
                case TOK_LBRACE :
                        return -1;
                        break;
                case TOK_LSBRACE :
                        return -1;
                        break;
                case TOK_SEMICOLON :
                        return -1;
                        break;
                case TOK_EOF :
                        return -1;
                        break;
                default :
                        perror ("Typing syntax error");
                        exit (EXIT_FAILURE);
                        break;
        }
}

Type* th_help (GUser_Types* G, Stream* S, int rbp)
{
        Token* curr_tok = stream_next(S);
        TokenType curr_type = curr_tok->type;
        Type* left_node = NULL;

        // nud conditionals
        if (curr_type == TOK_LPAREN) {
                left_node = th_help (G, S, 0);
                if (stream_next(S)->type != TOK_RPAREN) {
                        perror ("wat");
                        exit (EXIT_FAILURE);
                }
        } else {
                type_nud_lookup (G, curr_tok);
                left_node = malloc (sizeof (Type));
                if (curr_type == TOK_INT_TYPE) {
                        left_node->kind = VALUE;
                        left_node->data.base = INT;
                } else if (curr_type == TOK_INT_MUT_TYPE) {
                        left_node->kind = VALUE;
                        left_node->data.base = INT_MUT;
                } else if (curr_type == TOK_CHAR_TYPE) {
                        left_node->kind = VALUE;
                        left_node->data.base = CHAR;
                } else if (curr_type == TOK_CHAR_MUT_TYPE) {
                        left_node->kind = VALUE;
                        left_node->data.base = CHAR_MUT;
                } else if (curr_type == TOK_IDENTIFIER) {
                        left_node->kind = USER;
                        left_node->data.user = get_GUser (G, curr_tok->lexeme);
                } else {
                        perror ("Could not match prim type");
                        exit (EXIT_FAILURE);
                }
        }


        // led loop
        int lbp = type_led_lookup (G, stream_peek (S));
        while (rbp < lbp) {
                curr_tok = stream_next(S);
                curr_type = curr_tok->type;

                if (curr_type == TOK_ARROW_TYPE) {
                        Type* fun = malloc (sizeof (Type));
                        fun->kind = FUNCTION;
                        fun->data.tree = malloc (sizeof (Type_Tree));
                        fun->data.tree->input = left_node;
                        fun->data.tree->output = th_help (G, S, lbp);
                        left_node = fun;
                } else if (curr_type == TOK_STAR) {
                        Type* pt = malloc (sizeof (Type));
                        pt->kind = POINTER;
                        pt->data.pointer = left_node;
                        left_node = pt;
                        stream_next(S);
                } else {
                        perror ("Error in led loop");
                        exit (EXIT_FAILURE);
                }

                lbp = type_led_lookup (G, stream_peek (S));
        }

        return left_node;
}

Type* type_handler (GUser_Types* G, Stream* S)
//Pratt Parsing Algorithm
{
        return th_help (G, S, 0);
}

// ========================================================================= //


// ========================================================================= //
// Modular code for handling function body
Astn** fun_body_handler (AST_Program* A, Stream* S);


// ========================================================================= //


// ========================================================================= //
void fun_handler (AST_Program* A, GUser_Types* G, Stream* S)
{
        TokenType curr_type = (stream_next (S))->type;

        Fun_Type* fun = malloc (sizeof (Fun_Type));

        if (curr_type != TOK_IDENTIFIER ||
            stream_curr(S)->lexeme == NULL) {
                perror ("yo you didn't write the function right\n");
                exit (EXIT_FAILURE);
        }
        
        fun->fun_name = strdup((stream_curr(S))->lexeme);
        printf ("adding function: '%s'\n", fun->fun_name);
        // now expecting the left parenthesis
        curr_type = (stream_next (S))->type;

        if (curr_type != TOK_LPAREN) {
                perror ("yo what are you doing with this function\n");
                exit (EXIT_FAILURE);
        }

        // initializing parameters
        curr_type = (stream_next (S))->type;
        fun->num_param = 0;
        fun->param_cap = 4;
        fun->params = malloc (sizeof (Var*) * fun->param_cap);

        // now expecting either a bunch of arguments or right parenthesis
        while (curr_type == TOK_IDENTIFIER) {
                // Add new parameter variable
                Var* variable = malloc (sizeof (Var));

                // No value until we call the function!
                variable->value = NULL;

                // Name of variable
                variable->name = strdup((stream_curr(S))->lexeme);
                
                // Now looking for colon
                curr_type = (stream_next (S))->type;
                if (curr_type != TOK_COLON) {
                        perror ("Didn't declare type of parameter\n");
                        exit (EXIT_FAILURE);
                }

                // Now looking for type
                variable->type = type_handler (G, S);

                fun_param_add (fun, variable);

                if (stream_peek(S)->type == TOK_COMMA) {
                        stream_next (S);
                }
                curr_type = (stream_next(S))->type;
        }

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

        // now expecting a type
        fun->ret_type = type_handler (G, S);

        // TODO: pass onto handling the body
        //fun->body = body_handler (A, S);


        program_add_fun (A, fun);
}

// ========================================================================= //


// ========================================================================= //
AST_Program* parse (Stream* S)
{
        AST_Program* A = new_Program ();
        GUser_Types* G = new_GUser ();

        while (!is_stream_end (S)) {
                print_token (stream_curr (S));
                if (stream_curr(S)->type == TOK_ERROR) {
                        perror ("Somehow got an error token fucker\n");
                        exit (EXIT_FAILURE);
                } else if ((stream_curr(S))->type == TOK_TYPEDEF) {
                        //typedef_handler (G, S);
                } else if ((stream_curr(S))->type == TOK_FN) {
                        fun_handler (A, G, S);
                }

                stream_next (S);
        }



        return A;
}

// ========================================================================= //
