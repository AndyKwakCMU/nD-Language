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
// Forward declaration
Body_Block* body_handler (AST_Program* A, GUser_Types* G, 
                          Fun_Type* F, Stream* S);

Astn* fun_call_handler (AST_Program* A, GUser_Types* G, 
                        Fun_Type* F, Stream* S)
{
        // TODO

}

/*
        expr_nud table
        highest to lowest
        
        2: *, -  (ptr deref and negative sign)
        0: literal, identifier


        expr_led table

        2: ->, . (pointer access)
        1: *, /  
        0: +, -
*/

int expr_led (TokenType t)
{
        switch (t) {
                case TOK_ARROW_TYPE :
                        return 5;
                case TOK_DOT :
                        return 5;
                case TOK_STAR :
                        return 3;
                case TOK_SLASH :
                        return 3;
                case TOK_PLUS :
                        return 1;
                case TOK_MINUS :
                        return 1;
                case TOK_RPAREN :
                        return -1;
        }
        return -1;
}

Astn* expr_help (AST_Program* A, GUser_Types* G, 
                   Fun_Type* F, Stream* S, int rbp)
{
        Token* curr_tok = stream_next(S);
        TokenType curr_type = curr_tok->type;
        Astn* left_node = NULL;

        // nud conditionals
        if (curr_type == TOK_LPAREN) { 
                left_node = binexp_help (A, G, F, S, 0);
                if (stream_next(S)->type != TOK_RPAREN) {
                        perror ("binary expression missing RPAREN");
                        exit (EXIT_FAILURE);
                }
        } else {
                left_node = malloc (sizeof (Astn));
                if (!left_node) {
                        perror ("astn node allocation fail in binexp_help\n");
                        exit (EXIT_FAILURE);
                }

                if (curr_type == TOK_INT_LITERAL) {
                        // number
                        int val = atoi (curr_tok->lexeme);
                        left_node->kind = NODE_LITERAL;
                        left_node->data.literal->type = INT;
                        left_node->data.literal->value.int_val = val;
                } else if (curr_type == TOK_IDENTIFIER) {
                        // variable or function call
                        if (isin_fun_varlist (F, curr_tok->lexeme)) {
                                // Identifier is a variable
                                left_node->kind = NODE_LITERAL;
                                left_node->data.literal->type = VAR;
                                left_node->data.literal->value.var = (
                                        fun_varlist_get_var (F, curr_tok->lexeme));
                        } else if (isin_program_fun (A, curr_tok->lexeme)) {
                                // Identifier is a function call
                                fun_call_handler (A, G, F, S);
                        } else {
                                // syntax error
                                // TODO
                        }
                } else if (curr_type == TOK_MINUS) {
                        // negative sign
                        left_node->kind = NODE_UNARY_EXPR;
                        left_node->data.unary->op = TOK_MINUS;
                        left_node->data.unary->arg = expr_help (A, G, F, S, 4);
                } else if (curr_type == TOK_STAR) {
                        // pointer deference
                        left_node->kind = NODE_UNARY_EXPR;
                        left_node->data.unary->op = TOK_STAR;
                        Astn* ptr = left_node;
                        while (stream_peek(S)->type == TOK_STAR) {
                                Astn* new = malloc (sizeof (Astn));
                                new->kind = NODE_UNARY_EXPR;
                                new->data.unary->op = '*';
                                ptr->data.unary->arg = new;
                                ptr = new;
                                stream_next (S);
                        }
                        curr_tok = stream_next (S);
                        curr_type = curr_tok->type;
                        ptr->data.unary->arg = expr_help (A, G, F, S, 4);
                } else {
                        // syntax error
                        // TODO
                }
        }

        // led loop
        int lbp = binary_expr_led (stream_peek(S));
        while (rbp < lbp) {
                curr_tok = stream_next (S);
                curr_type = curr_tok->type;

                Astn* new = malloc (sizeof (Astn));
                if (!new) {
                        perror ("astn node allocation fail in binexp_help\n");
                        exit (EXIT_FAILURE);
                }

                new->kind = NODE_BINARY_EXPR;
                new->data.binary->op = curr_tok;

                new->data.binary->left = left_node;
                new->data.binary->right = binexp_help (A, G, F, S, lbp);
                left_node = new;
                lbp = binary_expr_led (stream_peek(S));
        }

        return left_node;
}

Astn* expr_handler (AST_Program* A, GUser_Types* G, 
                                  Fun_Type* fun, Stream* S)
{
        return expr_help (A, G, fun, S, 0);
}

Astn* body_cond_handler (AST_Program* A, GUser_Types* G, 
                         Fun_Type* fun, Stream* S)
{
        Astn* node = malloc (sizeof (Astn));
        node->kind = NODE_COND;
        Cond_Expr* cond = malloc (sizeof (Cond_Expr));

        TokenType curr_type = stream_curr(S)->type;
        if (curr_type == TOK_IF) {
                cond->kind = IF;
                curr_type = stream_next(S)->type;
                if (curr_type != TOK_LPAREN) {
                        perror ("conditional not following parenthesis");
                        exit (EXIT_FAILURE);
                }
                cond->cond = expr_handler (A, G, fun, S);
                curr_type = stream_next(S)->type;
                if (curr_type != TOK_RPAREN) {
                        perror ("conditional not closed by parenthesis");
                        exit (EXIT_FAILURE);
                }
                cond->body = body_handler (A, G, fun, S);
        } 
        curr_type = stream_next(S)->type;
        Cond_Expr* curr_pt = cond;
        while (curr_type == TOK_ELSEIF) {
                Cond_Expr* new = malloc (sizeof (Cond_Expr));
                new->kind = ELSEIF;
                if (curr_type != TOK_LPAREN) {
                        perror ("conditional not following parenthesis");
                        exit (EXIT_FAILURE);
                }
                new->cond = expr_handler (A, G, fun, S);
                if (curr_type != TOK_RPAREN) {
                        perror ("conditional not closed by parenthesis");
                        exit (EXIT_FAILURE);
                }
                new->body = body_handler (A, G, fun, S);
                curr_pt->chain = new;
                curr_pt = new;
                curr_type = stream_next(S)->type;
        }

        if (curr_type == TOK_ELSE) {
                Cond_Expr* new = malloc (sizeof (Cond_Expr));
                new->kind = ELSE;
                if (curr_type != TOK_LPAREN) {
                        perror ("conditional not following parenthesis");
                        exit (EXIT_FAILURE);
                }
                new->cond = expr_handler (A, G, fun, S);
                if (curr_type != TOK_RPAREN) {
                        perror ("conditional not closed by parenthesis");
                        exit (EXIT_FAILURE);
                }
                new->body = body_handler (A, G, fun, S);
                new->chain = NULL;
                curr_pt->chain = new;
        }

        // Should be a null/ELSE-kind terminated linked list

        node->data.cond = cond;
        return node;
}

Astn* body_loop_handler (AST_Program* A, GUser_Types* G, 
                         Fun_Type* fun, Stream* S)
{
        Astn* node = malloc (sizeof (Astn));
        node->kind = NODE_LOOP;
        Loop_Expr* loop = malloc (sizeof (Loop_Expr));

        loop->cond = binary_expr_handler(A, G, fun, S);
        loop->body = body_handler (A, G, fun, S);

        node->data.loop = loop;
        return node;
}

Astn* body_idt_handler (AST_Program* A, GUser_Types* G, 
                       Fun_Type* fun, Stream* S)
{
        char* idt_name = stream_curr(S)->lexeme;
        if (isin_fun_varlist (fun, idt_name)) {
                // the identifier is a variable
                
        } else if (program_search_fun(A, idt_name)) {
                // the identifier is a function

        } else {
                perror ("undefined identifier\n");
                exit (EXIT_FAILURE);
        }
}

Body_Block* body_handler (AST_Program* A, GUser_Types* G, 
                              Fun_Type* fun, Stream* S)
{
        Body_Block* B = new_body ();
        TokenType curr_type = stream_curr (S);

        // Square brace variable declaration body stuff
        if (curr_type == TOK_RSBRACE) {
                curr_type = stream_next(S)->type;
                while (curr_type == TOK_IDENTIFIER) {
                        // Declare new variables
                        Var* variable = malloc (sizeof (Var));

                        // Name of variable
                        variable->name = strdup((stream_curr(S))->lexeme);
                        
                        // Now looking for colon
                        curr_type = (stream_next (S))->type;
                        if (curr_type != TOK_COLON) {
                                perror ("Didn't declare type of variable\n");
                                exit (EXIT_FAILURE);
                        }

                        // Now looking for type
                        variable->type = type_handler (G, S);

                        variable->value = value_handler (G, fun, S);

                        body_add_var (B, variable);

                        if (stream_peek(S)->type == TOK_SEMICOLON) {
                                stream_next (S);
                        }
                        curr_type = (stream_next(S))->type;
                }
        }
        program_add_varlist (A, body_get_varlist(B));
        


        if (stream_curr(S)->type != TOK_LBRACE) {
                perror ("Body not declared!\n");
                exit (EXIT_FAILURE);
        }

        // Actual logic and computation inside braces
        curr_type = stream_next(S)->type;


        while (curr_type != TOK_RBRACE) {
                if (curr_type == TOK_IF) {
                        // Conditional Handler
                        Astn* node = body_cond_handler (A, G, fun, S);
                        body_add_inst (B, node);
                } else if (curr_type == TOK_WHILE) {
                        // Loop Handler
                        Astn* node = body_loop_handler (A, G, fun, S);
                        body_add_inst (B, node);
                } else if (curr_type == TOK_IDENTIFIER) {
                        // Can either be variable mutation,
                        // variable Assignment,function call,
                        // struct mutation, etc.
                        char* idt_name = stream_curr(S)->lexeme;
                        if (!idt_name) {
                                perror ("identifier token missing lexeme\n");
                                exit (EXIT_FAILURE);
                        }
                        Astn* node = body_idt_handler (A, G, fun, S);
                        body_add_inst (B, node);
                } else if (curr_type == TOK_LSBRACE) {
                        // New Body declaration, with new variables for its scope
                        Astn* node = malloc (sizeof (Astn));
                        node->kind = NODE_BODY;
                        node->data.body_block = body_handler (A, G, fun, S);
                        body_add_inst (B, node);
                } else if (curr_type == TOK_LBRACE) {
                        // New Body declaration, no new variables
                        Astn* node = malloc (sizeof (Astn));
                        node->kind = NODE_BODY;
                        node->data.body_block = body_handler (A, G, fun, S);
                        body_add_inst (B, node);
                } else {
                        perror ("Something went wrong in the body\n");
                        exit (EXIT_FAILURE);
                }
                // All of these cases should end at }
                curr_type = stream_next(S)->type;
        }


        return B;
}


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
        fun->variables = new_varlist ();

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

                fun_add_param (fun, variable);

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
        fun->body = body_handler (A, G, fun, S);


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
