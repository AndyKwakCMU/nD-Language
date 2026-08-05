// ========================================================================= //
// Andy Kwak 2026

// My parser implementation, top-down AST generator

// ========================================================================= //

// ========================================================================= //
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <assert.h>

#include "../tokenizer/token.h"
#include "../tokenizer/real_stream.h"
#include "../tokenizer/tokenize.h"

#include "../utils.h"

#include "parser.h"
#include "ast.h"
#include "ast_util.h"

// ========================================================================= //
AST_Program* parse (Stream* S);


void typedef_handler (AST_Program* A, GUser_Types* G, Stream* S);
Type* type_handler (GUser_Types* G, Stream* S);
Astn* expr_handler (AST_Program* A, GUser_Types* G, 
                    Fun_Type* fun, Stream* S);


// ========================================================================= //
Var_List* user_struct_handler (AST_Program* A, GUser_Types* G, Stream* S)
{
        #ifdef DEBUG
        printf ("user struct handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif

        Var_List* V = new_varlist ();

        // should start at the TOK_LBRACE
        while (stream_peek(S)->type != TOK_RBRACE) {
                stream_next(S); // now at an identifier

                #ifdef DEBUG
                printf ("user struct handler debug print token 2\n");
                print_token (stream_curr(S));
                #endif

                if (stream_curr(S)->type != TOK_IDENTIFIER) {
                        serr (stream_curr(S), "user struct declaration missing target identifier");
                }
                if (!stream_curr(S)->lexeme) {
                        serr (stream_curr(S), "user struct declaration missing target lexeme");
                }

                Var* var = malloc (sizeof (Var));
                var->name = strdup (stream_curr(S)->lexeme);
                
                stream_next(S); // now at a colon
                if (stream_curr(S)->type != TOK_COLON) {
                        serr (stream_curr(S), "user struct declaration missing type");
                }

                var->type = type_handler (G, S);

                #ifdef DEBUG
                printf ("user struct handler debug print token 3\n");
                print_token (stream_curr(S));
                #endif

                stream_next(S); // now at = or ;
                if (stream_curr(S)->type == TOK_ASSIGN) {
                        #ifdef DEBUG
                        printf ("user struct handler debug print token 4\n");
                        print_token (stream_curr(S));
                        #endif
                        Fun_Type* F = new_fun(NULL);
                        fun_var_dec_add (F, V);
                        var->value = expr_handler (A, G, F, S);
                        #ifdef DEBUG
                        printf ("user struct handler debug print token 5\n");
                        print_token (stream_curr(S));
                        #endif

                        fun_var_rem (F);


                        stream_next(S); // now should be semicolon
                } 

                #ifdef DEBUG
                printf ("user struct handler debug print token 6\n");
                print_token (stream_curr(S));
                #endif
                
                if (stream_curr(S)->type != TOK_SEMICOLON) {
                        serr (stream_curr(S), "syntax error at user struct declaration");
                } 

                varlist_add (V, var);
        }

        return V;
}

void typedef_handler (AST_Program* A, GUser_Types* G, Stream* S)
{
        stream_next(S);

        #ifdef DEBUG
        printf ("typedef handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif

        if (stream_curr(S)->type != TOK_IDENTIFIER) {
                serr (stream_curr(S), "typedef missing target identifier");
        }
        if (!stream_curr(S)->lexeme) {
                // this is impossible to happen
                serr (stream_curr(S), "target identifier missing lexeme");
        }

        char* name = strdup (stream_curr(S)->lexeme);

        User_Type* U;
        if (isin_GUser (G, name)) {
                U = get_GUser (G, name);
        } else {
                U = malloc (sizeof (User_Type));
                U->name = name;
        }
        stream_next(S);

        #ifdef DEBUG
        printf ("typedef handler debug print token 2\n");
        print_token (stream_curr(S));
        #endif

        if (stream_curr(S)->type == TOK_SEMICOLON) {
                // Forward declaration case
                if (!isin_GUser (G, name)) {
                     U->kind = HOLD;   
                }
        } else {
                #ifdef DEBUG
                printf ("typedef handler debug print token 3\n");
                print_token (stream_curr(S));
                #endif
                if (stream_curr(S)->type != TOK_ASSIGN) {
                        serr (stream_curr(S), "typedef missing assignment");
                }

                if (stream_peek(S)->type == TOK_LBRACE) {
                        stream_next(S);
                        #ifdef DEBUG
                        printf ("typedef handler debug print token 4\n");
                        print_token (stream_curr(S));
                        #endif
                        U->kind = STRUCT;
                        U->data.Struct = user_struct_handler (A, G, S);
                        if (stream_next(S)->type != TOK_RBRACE) {
                                serr (stream_curr(S), "typedef alias missing closing brace");
                        }

                } else {
                        U->kind = ALIAS;
                        U->data.Alias = type_handler (G, S);

                        #ifdef DEBUG
                        printf ("typedef handler debug print token 5\n");
                        print_token (stream_curr(S));
                        #endif

                        if (stream_next(S)->type != TOK_SEMICOLON) {
                                serr (stream_curr(S), "typedef alias missing semicolon");
                        }

                }
        }

        #ifdef DEBUG
        printf ("typedef handler debug print token 2\n");
        print_token (stream_curr(S));
        #endif
        
        if (!isin_GUser (G, name)) {
                GUser_add (G, U);
        }


        #ifdef DEBUG
        printf ("current state of GUSER:\n");
        print_GUser (G);
        #endif
}

// ========================================================================= //
/*
int type_nud_lookup (GUser_Types* G, Token* tok)
{
        TokenType tok_type = tok->type;
        switch (tok_type) {
                case TOK_INT_TYPE :
                        return 0;
                case TOK_CHAR_TYPE :
                        return 0;
                case TOK_STRING_TYPE :
                        return 0;
                case TOK_LIST_TYPE :
                        return 0;
                case TOK_IDENTIFIER :
                        if (isin_GUser (G, tok->lexeme)) {
                                return 0;
                        } 
                default :
                        serr (tok, "token not in nud");
        }
        return 0;
}
*/

int type_led_lookup (GUser_Types* G, Token* tok)
{
        TokenType tok_type = tok->type;
        switch (tok_type) {
                case TOK_ARROW_TYPE :
                        return 100;
                case TOK_STAR :
                        return 200;
                case TOK_MUT :
                        return 200;
                case TOK_RPAREN :
                        return -1;
                case TOK_COMMA :
                        return -1;
                case TOK_LBRACE :
                        return -1;
                case TOK_LSBRACE :
                        return -1;
                case TOK_SEMICOLON :
                        return -1;
                case TOK_EOF :
                        return -1;
                case TOK_ASSIGN :
                        return -1;
                default :
                        return -1;
        }
        return -1;
}

Type* th_help (GUser_Types* G, Stream* S, int rbp)
{
        Token* curr_tok = stream_next(S);
        TokenType curr_type = curr_tok->type;
        Type* left_node = NULL;

        #ifdef DEBUG
        printf ("type handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif

        // nud conditionals
        if (curr_type == TOK_LPAREN) {
                left_node = th_help (G, S, 0);
                if (stream_next(S)->type != TOK_RPAREN) {
                        serr (stream_curr(S), "type parenthesis not closed");
                }
        } else {
                left_node = malloc (sizeof (Type));
                if (curr_type == TOK_INT_TYPE) {
                        left_node->kind = VALUE;
                        left_node->data.base = INT;
                        left_node->size = sizeof (int);
                } else if (curr_type == TOK_CHAR_TYPE) {
                        left_node->kind = VALUE;
                        left_node->data.base = CHAR;
                        left_node->size = sizeof (char);
                } else if (curr_type == TOK_STRING_TYPE) {
                        left_node->kind = VALUE;
                        left_node->data.base = STRING;
                } else if (curr_type == TOK_LIST_TYPE) {
                        left_node->kind = LIST;
                        left_node->data.list = type_handler (G, S);
                } else if (curr_type == TOK_NONE_TYPE) {
                        left_node->kind = NONE;
                        left_node->data.pointer = NULL;
                } else if (curr_type == TOK_IDENTIFIER) {
                        left_node->kind = USER;
                        if (isin_GUser (G, stream_curr(S)->lexeme)) {
                                left_node->data.user = get_GUser (G, curr_tok->lexeme);
                        } else {
                                serr (stream_curr(S), "user type undeclared");
                        }
                        
                } else {
                        serr (stream_curr(S), "type identifier undeclared");
                }
        }

        #ifdef DEBUG
        printf ("type handler debug print token 2\n");
        print_token (stream_curr(S));
        #endif


        // led loop
        int lbp = type_led_lookup (G, stream_peek (S));
        while (rbp < lbp) {
                curr_tok = stream_next(S);
                curr_type = curr_tok->type;

                #ifdef DEBUG
                printf ("type handler debug print token 3\n");
                print_token (stream_curr(S));
                #endif

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
                } else if (curr_type == TOK_MUT) {
                        Type* pt = malloc (sizeof (Type));
                        pt->kind = MUTABLE;
                        pt->data.mutable = left_node;
                        left_node = pt;
                } else {
                        serr (stream_curr(S), "invalid binary type syntax");
                }

                #ifdef DEBUG
                printf ("type handler debug print token 4\n");
                print_token (stream_curr(S));
                #endif

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

Astn* expr_handler (AST_Program* A, GUser_Types* G, 
                    Fun_Type* fun, Stream* S);

void fun_call_arg_handler (AST_Program* A, GUser_Types* G, 
                            Fun_Type* F, Stream* S, Fun_Call* C)
{
        #ifdef DEBUG
        printf ("fun call arg handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif
        while (stream_peek(S)->type != TOK_RPAREN) {
                #ifdef DEBUG
                printf ("fun call arg handler debug print token 2\n");
                print_token (stream_curr(S));
                #endif
                // expr_handler will keep going through tokens until it hits
                // the seperating commas, which then will come back here and
                // be added to the arguments. 
                call_add_arg (C, expr_handler (A, G, F, S));

                #ifdef DEBUG
                printf ("fun call arg handler debug print token 3\n");
                print_token (stream_curr(S));
                #endif

                if (stream_peek(S)->type == TOK_COMMA) {
                        stream_next (S); // consume the separating comma
                } else if (stream_peek(S)->type != TOK_RPAREN) {
                        // Syntax error
                        serr (stream_curr(S), "function call arg syntax error");
                }
        }

        if (stream_next(S)->type != TOK_RPAREN) {
                // function arg not closed, syntax error
                serr (stream_curr(S), "function call arg not closed");
        }
}

Fun_Call* fun_call_handler (AST_Program* A, GUser_Types* G, 
                            Fun_Type* F, Stream* S)
{
        #ifdef DEBUG
        printf ("fun call handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif

        Fun_Call* C = new_call (stream_curr(S)->lexeme);
        
        if (stream_peek(S)->type == TOK_LPAREN) {
                // function name should be followed by parenthesis and args
                stream_next(S);
                fun_call_arg_handler (A, G, F, S, C);
                #ifdef DEBUG
                printf ("fun call handler debug print token 2\n");
                print_token (stream_curr(S));
                #endif
        }

        // simply the function as value
        #ifdef DEBUG
        printf ("fun call handler debug print token 3\n");
        print_token (stream_curr(S));
        #endif

        return C;
}

Lambda_Expr* lambda_handler (AST_Program* A, GUser_Types* G, 
                            Fun_Type* F, Stream* S)
{
        #ifdef DEBUG
        printf ("lambda handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif


        Lambda_Expr* L = malloc (sizeof (Lambda_Expr));
        if (!L) {
                aerr (stream_curr(S));
        }


        L->var = malloc (sizeof (Var));
        if (!L->var) {
                aerr (stream_curr(S));
        }

        Token* tok = stream_next (S);
        
        #ifdef DEBUG
        printf ("lambda handler debug print token 2\n");
        print_token (stream_curr(S));
        #endif
        
        L->var->name = strdup (tok->lexeme);
        tok = stream_next (S);
        
        if (tok->type != TOK_COLON) {
                serr (stream_curr(S), "lambda arg type missing");
        }

        L->var->type = type_handler (G, S);
        
        #ifdef DEBUG
        printf ("lambda handler debug print token 3\n");
        print_token (stream_curr(S));
        #endif
        
        L->var->value = NULL;

        Var_List* V = new_varlist ();

        varlist_add (V, L->var);

        fun_var_dec_add (F, V);

        tok = stream_next (S);
        if (tok->type != TOK_MATCH_ARROW) {
                serr (stream_curr(S), "lambda missing match arrow");
        }

        L->function = expr_handler (A, G, F, S);

        #ifdef DEBUG
        printf ("lambda handler debug print token 4\n");
        print_token (stream_curr(S));
        #endif

        free (fun_var_rem (F)); // DO NOT FREE THE VARIABLES

        return L;
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
                case TOK_LPAREN :
                        return 8;
                case TOK_ARROW_TYPE :
                        return 7;
                case TOK_DOT :
                        return 7;
                case TOK_STAR :
                        return 6;
                case TOK_SLASH :
                        return 6;
                case TOK_PLUS :
                        return 5;
                case TOK_MINUS :
                        return 5;
                case TOK_EQ :
                        return 4;
                case TOK_LT :
                        return 4;
                case TOK_LEQ :
                        return 4;
                case TOK_GT :
                        return 4;
                case TOK_GEQ :
                        return 4;
                case TOK_AND :
                        return 3;
                case TOK_OR :
                        return 2;
                case TOK_ASSIGN :
                        return 1;
                case TOK_ADD_ASSIGN :
                        return 1;
                case TOK_SUB_ASSIGN :
                        return 1;
                case TOK_MUL_ASSIGN :
                        return 1;
                case TOK_DIV_ASSIGN :
                        return 1;
                default :
                        return -1;
        }
}

Astn* expr_help (AST_Program* A, GUser_Types* G, 
                 Fun_Type* F, Stream* S, int rbp)
{
        Token* curr_tok = stream_next(S);

        #ifdef DEBUG
        printf ("expr handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif

        TokenType curr_type = curr_tok->type;
        Astn* left_node = NULL;

        // nud conditionals
        if (curr_type == TOK_LPAREN) { 
                left_node = expr_help (A, G, F, S, 0);
                if (stream_next(S)->type != TOK_RPAREN) {
                        serr (stream_curr(S), "expr missing closing RPAREN");
                }
        } else {
                left_node = malloc (sizeof (Astn));
                if (!left_node) {
                        aerr (stream_curr(S));
                }

                if (curr_type == TOK_INT_LITERAL) {
                        // number
                        int val = atoi (curr_tok->lexeme);
                        left_node->kind = NODE_LITERAL;
                        left_node->data.literal = malloc (sizeof (Literal_Expr));
                        if (!left_node->data.literal) {
                                aerr (stream_curr(S));
                        }
                        left_node->data.literal->kind = LIT_INT;
                        Type* type = malloc (sizeof (Type));
                        if (!type) {
                                aerr (stream_curr(S));
                        }
                        type->kind = VALUE;
                        type->data.base = INT;
                        left_node->data.literal->type = type;
                        left_node->data.literal->value.int_val = val;
                } else if (curr_type == TOK_NEW) {
                        left_node->kind = NODE_LITERAL;
                        left_node->data.literal = malloc (sizeof (Literal_Expr));
                        if (!left_node->data.literal) {
                                aerr (stream_curr(S));
                        }
                        left_node->data.literal->kind = LIT_NEW;
                        left_node->data.literal->value.var = NULL;
                } else if (curr_type == TOK_IDENTIFIER) {
                        // variable or function call

                        ASSERT (stream_curr(S)->lexeme != NULL);

                        if (isin_fun_varlist (F, curr_tok->lexeme)) {
                                // Identifier is a variable
                                left_node->kind = NODE_LITERAL;
                                left_node->data.literal = malloc (sizeof (Literal_Expr));
                                if (!left_node->data.literal) {
                                        aerr (stream_curr(S));
                                }
                                left_node->data.literal->kind = LIT_VAR;
                                left_node->data.literal->value.var = 
                                                fun_varlist_get_var (F, curr_tok->lexeme);
                        } else if (isin_program_fun (A, curr_tok->lexeme)) {
                                // Identifier is a function call
                                left_node->kind = NODE_FUN_CALL;
                                left_node->data.fun_call = fun_call_handler (A, G, F, S);
                        } else {
                                // Identifier is a variable inside a struct
                                // maybe... but we cant check here!
                                left_node->kind = NODE_LITERAL;
                                left_node->data.literal = malloc (sizeof (Literal_Expr));
                                if (!left_node->data.literal) {
                                        aerr (stream_curr(S));
                                }
                                left_node->data.literal->kind = LIT_VAR;
                                left_node->data.literal->value.var = 
                                                malloc (sizeof (Var));
                                left_node->data.literal->value.var->name = 
                                                strdup (stream_curr(S)->lexeme);
                                left_node->data.literal->value.var->type = NULL;
                                left_node->data.literal->value.var->value = NULL;
                        }
                } else if (curr_type == TOK_MINUS) {
                        // negative sign
                        left_node->kind = NODE_UNARY_EXPR;
                        left_node->data.unary = malloc (sizeof (Unary_Expr));
                        left_node->data.unary->op = TOK_MINUS;
                        left_node->data.unary->arg = expr_help (A, G, F, S, 4);
                } else if (curr_type == TOK_STAR) {
                        // pointer deference
                        left_node->kind = NODE_UNARY_EXPR;
                        left_node->data.unary = malloc (sizeof (Unary_Expr));
                        left_node->data.unary->op = TOK_STAR;
                        Astn* ptr = left_node;
                        while (stream_peek(S)->type == TOK_STAR) {
                                Astn* new = malloc (sizeof (Astn));
                                new->kind = NODE_UNARY_EXPR;
                                new->data.unary = malloc (sizeof (Unary_Expr));
                                new->data.unary->op = TOK_STAR;
                                ptr->data.unary->arg = new;
                                ptr = new;
                                stream_next (S);
                        }
                        ptr->data.unary->arg = expr_help (A, G, F, S, 4);
                } else if (curr_type == TOK_RETURN) {
                        left_node->kind = NODE_UNARY_EXPR;
                        left_node->data.unary = malloc (sizeof (Unary_Expr));
                        left_node->data.unary->op = TOK_RETURN;
                        left_node->data.unary->arg = expr_help (A, G, F, S, 0);
                } else if (curr_type == TOK_LAMBDA) {
                        // TODO : Lambda function
                        left_node->kind = NODE_LAMBDA;
                        left_node->data.lambda = lambda_handler (A, G, F, S);
                } else {
                        // syntax error
                        serr (stream_curr(S), "expr syntax error");
                }
        }

        #ifdef DEBUG
        printf ("expr handler debug print token 2\n");
        print_token (stream_curr(S));
        #endif

        // led loop
        int lbp = expr_led (stream_peek(S)->type);
        while (rbp < lbp) {
                curr_tok = stream_next (S);
                curr_type = curr_tok->type;

                #ifdef DEBUG
                printf ("expr handler debug print token 3\n");
                print_token (stream_curr(S));
                #endif

                Astn* new = malloc (sizeof (Astn));
                if (!new) {
                        aerr (stream_curr(S));
                }

                if (curr_type == TOK_LPAREN) {
                        new->kind = NODE_LAMCALL;
                        new->data.lam_call = malloc (sizeof (Lambda_Call));
                        if (!new->data.lam_call) {
                                aerr (curr_tok);
                        }
                        
                        new->data.lam_call->function = left_node;
                        new->data.lam_call->arg = expr_help (A, G, F, S, 0);
                        curr_tok = stream_next(S);
                        curr_type = curr_tok->type;
                        if (curr_type != TOK_RPAREN) {
                                serr (curr_tok, "lambda function call parenthesis not closed");
                        }
                        left_node = new;
                } else {
                        new->kind = NODE_BINARY_EXPR;
                        new->data.binary = malloc (sizeof (Binary_Expr));
                        new->data.binary->op = curr_type;


                        new->data.binary->left = left_node;
                        new->data.binary->right = expr_help (A, G, F, S, lbp);
                        left_node = new;
                }

                #ifdef DEBUG
                printf ("expr handler debug print token 4\n");
                print_token (stream_curr(S));
                #endif

                lbp = expr_led (stream_peek(S)->type);
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
        #ifdef DEBUG
        printf ("body cond handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif

        Astn* node = malloc (sizeof (Astn));
        node->kind = NODE_COND;
        Cond_Expr* cond = malloc (sizeof (Cond_Expr));

        TokenType curr_type = stream_curr(S)->type;

        REQUIRES (curr_type == TOK_IF);

        #ifdef DEBUG
        printf ("body cond handler debug print token 2\n");
        print_token (stream_curr(S));
        #endif

        cond->kind = IF;
        curr_type = stream_next(S)->type;

        if (curr_type != TOK_LPAREN) {
                serr (stream_curr(S), "if statement missing parenthesis");
        }

        cond->cond = expr_handler (A, G, fun, S);

        #ifdef DEBUG
        printf ("body cond handler debug print token 3\n");
        print_token (stream_curr(S));
        #endif

        curr_type = stream_next(S)->type;

        if (curr_type != TOK_RPAREN) {
                serr (stream_curr(S), "if statement missing closing parenthesis");
        }

        curr_type = stream_next(S)->type;
        cond->body = body_handler (A, G, fun, S);

        #ifdef DEBUG
        printf ("body cond handler debug print token 4\n");
        print_token (stream_curr(S));
        #endif

        curr_type = stream_next(S)->type;

        if (curr_type != TOK_RBRACE) {
                serr (stream_curr(S), "if statement body missing closing brace");
        }

        // curr is now ON the if-body's closing brace (consumed). Peek ahead
        // (without consuming) to see if an elseif/else chain follows, so a
        // bare "if" doesn't over-advance past the token after it.
        Cond_Expr* curr_pt = cond;

        #ifdef DEBUG
        printf ("body cond handler debug print token 5\n");
        print_token (stream_curr(S));
        #endif

        while (stream_peek(S)->type == TOK_ELSEIF) {
                stream_next(S); // curr = TOK_ELSEIF

                Cond_Expr* new = malloc (sizeof (Cond_Expr));
                new->kind = ELSEIF;
                curr_type = stream_next(S)->type;

                if (curr_type != TOK_LPAREN) {
                        serr (stream_curr(S), "elseif statement missing parenthesis");
                }

                new->cond = expr_handler (A, G, fun, S);

                #ifdef DEBUG
                printf ("body cond handler debug print token 6\n");
                print_token (stream_curr(S));
                #endif

                curr_type = stream_next(S)->type;

                if (curr_type != TOK_RPAREN) {
                        serr (stream_curr(S), "elseif statement missing closing parenthesis");
                }

                curr_type = stream_next(S)->type;
                new->body = body_handler (A, G, fun, S);

                #ifdef DEBUG
                printf ("body cond handler debug print token 7\n");
                print_token (stream_curr(S));
                #endif

                curr_pt->chain = new;
                curr_pt = new;
                curr_type = stream_next(S)->type;

                if (curr_type != TOK_RBRACE) {
                        serr (stream_curr(S), "elseif statement body missing closing brace");
                }
        }

        if (stream_peek(S)->type == TOK_ELSE) {
                stream_next(S); // curr = TOK_ELSE

                Cond_Expr* new = malloc (sizeof (Cond_Expr));
                new->kind = ELSE;
                new->cond = NULL; // ELSE has no condition of its own
                stream_next(S);
                new->body = body_handler (A, G, fun, S);

                #ifdef DEBUG
                printf ("body cond handler debug print token 8\n");
                print_token (stream_curr(S));
                #endif

                new->chain = NULL;
                curr_pt->chain = new;
                curr_type = stream_next(S)->type;
                if (curr_type != TOK_RBRACE) {
                        serr (stream_curr(S), "elseif statement body missing closing brace");
                }
        } else {
                curr_pt->chain = NULL;
        }

        // Should be a null/ELSE-kind terminated linked list
        // Postcondition: curr is on the closing brace of the last processed
        // clause's body, matching the "curr on last token of statement"
        // convention used elsewhere in body_handler.

        node->data.cond = cond;
        return node;
}

Astn* body_loop_handler (AST_Program* A, GUser_Types* G, 
                         Fun_Type* fun, Stream* S)
{
        #ifdef DEBUG
        printf ("body loop handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif

        Astn* node = malloc (sizeof (Astn));
        node->kind = NODE_LOOP;
        Loop_Expr* loop = malloc (sizeof (Loop_Expr));

        loop->cond = expr_handler(A, G, fun, S);

        #ifdef DEBUG
        printf ("body loop handler debug print token 2\n");
        print_token (stream_curr(S));
        #endif

        TokenType curr_type = stream_curr(S)->type;
        if (curr_type != TOK_RPAREN) {
                serr (stream_curr(S), "loop statement condition missing closing parenthesis");
        }

        stream_next(S);

        #ifdef DEBUG
        printf ("body loop handler debug print token 3\n");
        print_token (stream_curr(S));
        #endif

        loop->body = body_handler (A, G, fun, S);

        #ifdef DEBUG
        printf ("body loop handler debug print token 4\n");
        print_token (stream_curr(S));
        #endif

        curr_type = stream_next(S)->type;

        #ifdef DEBUG
        printf ("body loop handler debug print token 5\n");
        print_token (stream_curr(S));
        #endif

        if (curr_type != TOK_RBRACE) {
                serr (stream_curr(S), "loop statement body missing closing brace");
        }

        node->data.loop = loop;
        return node;
}

Body_Block* body_handler (AST_Program* A, GUser_Types* G, 
                              Fun_Type* fun, Stream* S)
{
        Body_Block* B = new_body ();
        TokenType curr_type = stream_curr (S)->type;

        #ifdef DEBUG
        printf ("body handler debug print token 1\n");
        print_token (stream_curr(S));
        #endif

        // Square brace variable declaration body stuff
        if (curr_type == TOK_LSBRACE) {
                // B->vars was already allocated (empty) by new_body ().
                stream_next(S);

                // still at TOK_LSBRACE, peeking...
                while (stream_curr(S)->type != TOK_RSBRACE) {
                        #ifdef DEBUG
                        printf ("body handler debug print token 2\n");
                        print_token (stream_curr(S));
                        #endif
                        
                        // Declare new variables
                        Var* variable = malloc (sizeof (Var));

                        // Name of variable
                        ASSERT (stream_curr(S)->lexeme != NULL);
                        variable->name = strdup((stream_curr(S))->lexeme);
                        
                        // Now looking for colon
                        curr_type = (stream_next (S))->type;
                        if (curr_type != TOK_COLON) {
                                serr (stream_curr(S), "body var dec missing type");
                        }

                        // Now looking for type
                        variable->type = type_handler (G, S);

                        #ifdef DEBUG
                        printf ("body handler debug print token 3\n");
                        print_token (stream_curr(S));
                        #endif

                        curr_type = stream_next(S)->type;

                        // If the variable is assigned a value
                        if (curr_type == TOK_ASSIGN) {
                                // now we are at the assign token
                                variable->value = expr_handler (A, G, fun, S);
                                #ifdef DEBUG
                                printf ("body handler debug print token 4\n");
                                print_token (stream_curr(S));
                                #endif
                                curr_type = stream_next(S)->type;
                        }

                        if (curr_type != TOK_SEMICOLON) {
                                // syntax error
                                serr (stream_curr(S), "body var dec missing semicolon");
                        }
                        body_add_var (B, variable);

                        curr_type = (stream_next(S))->type;
                }
                stream_next(S);
        }
        fun_var_dec_add (fun, body_get_varlist(B));

        #ifdef DEBUG
        printf ("body handler debug print token 5\n");
        print_token (stream_curr(S));
        #endif


        if (stream_curr(S)->type != TOK_LBRACE) {
                serr (stream_curr(S), "body missing body");
        }

        // Actual logic and computation inside braces
        TokenType peek = stream_peek (S)->type;


        while (peek != TOK_RBRACE) {
                #ifdef DEBUG
                printf ("body handler debug print token 6\n");
                print_token (stream_curr(S));
                #endif
                if (peek == TOK_IF) {
                        // Conditional Handler
                        stream_next (S);
                        Astn* node = body_cond_handler (A, G, fun, S);
                        body_add_inst (B, node);
                } else if (peek == TOK_WHILE) {
                        // Loop Handler
                        stream_next (S);
                        Astn* node = body_loop_handler (A, G, fun, S);
                        body_add_inst (B, node);
                } else if (peek == TOK_LSBRACE) {
                        // New Body declaration, with new variables for its scope
                        stream_next (S);
                        Astn* node = malloc (sizeof (Astn));
                        node->kind = NODE_BODY;
                        node->data.body_block = body_handler (A, G, fun, S);
                        if (stream_next(S)->type != TOK_RBRACE) {
                                serr (stream_curr(S), "nested body missing closing brace");
                        }
                        body_add_inst (B, node);
                } else if (peek == TOK_LBRACE) {
                        stream_next (S);
                        // New Body declaration, no new variables
                        Astn* node = malloc (sizeof (Astn));
                        node->kind = NODE_BODY;
                        node->data.body_block = body_handler (A, G, fun, S);
                        if (stream_next(S)->type != TOK_RBRACE) {
                                serr (stream_curr(S), "nested body missing closing brace");
                        }
                        body_add_inst (B, node);
                } else {
                        Astn* node = expr_handler (A, G, fun, S);
                        stream_next(S);

                        if (stream_curr(S)->type != TOK_SEMICOLON) {
                                // Syntax error
                                serr (stream_curr(S), "statement missing semicolon in body");
                        }
                        body_add_inst (B, node);
                }

                #ifdef DEBUG
                printf ("body handler debug print token 7\n");
                print_token (stream_curr(S));
                #endif

                // All of these cases should end at } eventually
                peek = stream_peek(S)->type;
        }

        fun_var_rem (fun);


        return B;
}


// ========================================================================= //


// ========================================================================= //
void fun_handler (AST_Program* A, GUser_Types* G, Stream* S)
{
        TokenType curr_type = (stream_next (S))->type;

        #ifdef DEBUG
        printf ("fun handler debug print token 1, looking for identifier\n");
        print_token (stream_curr(S));
        #endif

        if (curr_type != TOK_IDENTIFIER ||
            stream_curr(S)->lexeme == NULL) {
                serr (stream_curr(S), "function name undeclared");
        }
        
        Fun_Type* fun = new_fun(strdup((stream_curr(S))->lexeme));
        
        #ifdef DEBUG
        printf ("adding function: '%s'\n", fun->fun_name);
        #endif
        
        // now expecting the left parenthesis
        curr_type = (stream_next (S))->type;
        
        #ifdef DEBUG 
        printf ("fun handler debug print token 2, looking for LPAREN\n");
        print_token (stream_curr(S));
        #endif

        if (curr_type != TOK_LPAREN) {
                serr (stream_curr(S), "function missing parenthesis");
        }

        curr_type = (stream_next (S))->type;

        

        // now expecting either a bunch of arguments or right parenthesis
        while (curr_type == TOK_IDENTIFIER) {
                
                #ifdef DEBUG
                printf ("fun handler debug print token 3, looking for identifier\n");
                print_token (stream_curr(S));
                #endif
                

                // Add new parameter variable
                Var* variable = malloc (sizeof (Var));

                // No value until we call the function!
                variable->value = NULL;

                // Name of variable
                variable->name = strdup((stream_curr(S))->lexeme);

                #ifdef DEBUG
                printf ("fun handler debug print token 4, looking for variable name in function param\n");
                printf ("function name: %s\n", variable->name);
                #endif
                
                // Now looking for colon
                curr_type = (stream_next (S))->type;

                #ifdef DEBUG
                printf ("fun handler debug print token 5, looking for colon\n");
                print_token (stream_curr(S));
                #endif

                if (curr_type != TOK_COLON) {
                        serr (stream_curr(S), "function parameter missing type");
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
                serr (stream_curr(S), "function parameter missing closing parenthesis");
        } 

        // now expecting colon, declaring the return type of the function.
        curr_type = (stream_next (S))->type;
        if (curr_type != TOK_COLON) {
                // Didn't have colon to declare return type.
                serr (stream_curr(S), "function missing return type");
        }

        // now expecting a type
        fun->ret_type = type_handler (G, S);

        stream_next(S);

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
                #ifdef DEBUG
                printf ("parse function while loop tokens\n");
                print_token (stream_curr (S));
                #endif

                if (stream_curr(S)->type == TOK_ERROR) {
                        serr (stream_curr(S), "token error");
                } else if ((stream_curr(S))->type == TOK_TYPEDEF) {
                        typedef_handler (A, G, S);
                } else if ((stream_curr(S))->type == TOK_FN) {
                        fun_handler (A, G, S);
                }

                stream_next (S);
        }

        A->G = G;

        return A;
}

// ========================================================================= //
