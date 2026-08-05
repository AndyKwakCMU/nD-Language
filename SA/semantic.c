// ========================================================================= //
// Andy Kwak 2026

// Semantic Analysis Implementation of the nD-Language
// This is where we rigorously type check, assign function arena size,
// define allocated memory scope, and complete some of the unfinished
// details of the AST.

/*

        Further details of the semantic analysis is that my AST with user structs
        mostly hold a placeholder, they need to be rigourous type checked as well
        as check for the existence of them. Alias types and such should be efficiently
        checked as well.

        Another set of concerns is checking if the type is mutable, and invalidating
        any mutations on non-mutable variables. This also applies to user struct
        variables.

        We also have partial evaluation of functions, that will be type checked,
        and make sure the output of the function, or the partial function, is
        still valid logically and semantically in the program.

*/
// ========================================================================= //

// ========================================================================= //
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <assert.h>

#include "../parser/ast.h"
#include "../tokenizer/token.h"
#include "../utils.h"

// ========================================================================= //

// ========================================================================= //
void semantic_analysis(AST_Program *A);

void fill_hold (AST_Program* A, GUser_Types* G, Astn* ast);

Type* ast_check (AST_Program* A, GUser_Types* G, Type* ret_type, Astn* ast);

Type* binary_check (AST_Program* A, GUser_Types* G, Type* ret_type, Binary_Expr* ast);

void body_check (AST_Program* A, GUser_Types* G, Type* ret_type, Body_Block* B);

// ========================================================================= //

// ========================================================================= //
bool is_fun(Fun_Type* F) 
{
        if (!F)
                return false;
        if (!F->fun_name)
                return false;
        if (!F->ret_type)
                return false;
        if (!F->variables)
                return false;
        if (!F->body)
                return false;

        // TODO: more rigourous check on Fun_Type

        return true;
}

// Making sure there is only one function of its name
void fun_name_check (AST_Program* A, char* name) 
{
        size_t i = 0;
        size_t n = A->function_count;

        int count = 0;

        while (i < n) {
                Astn *ast = A->functions[i++];
                if (ast->kind != NODE_FUN_DEC) {
                        saerr("AST_Program functions list does not have function delcaration");
                }

                Fun_Type *F = ast->data.fun_dec;
                if (strcmp(name, F->fun_name) == 0)
                        count++;
        }

        if (count > 1) {
                printf ("Too many functions of the name %s is in AST_Program", name);
                saerr ("");
        } else if (count < 1) {
                printf ("No functions of the name %s is in AST_Program", name);
                saerr ("");
        } // else (count == 1) do nothing
}

Type* literal_check (Literal_Expr* L)
{
        if (L->kind == LIT_INT) {
                return L->type;
        } else if (L->kind == LIT_VAR) {
                return L->value.var->type;
        }
        return NULL;
}

Type* binary_check_int (AST_Program* A, GUser_Types* G, Type* ret_type, Binary_Expr* B)
{
        Type* left  = ast_check (A, G, ret_type, B->left);
        Type* right = ast_check (A, G, ret_type, B->right);

        Type* integer_type = malloc (sizeof (Type));
        if (!integer_type) {
                printf ("test integer type allocation failed\n");
                exit (EXIT_FAILURE);
        }
        integer_type->kind = VALUE;
        integer_type->data.base = INT;
        if (!typecmp (left, right)) {
                saerr ("conflicting types\n");
        }
        if (!typecmp (left, integer_type)) {
                saerr ("Arithmetic on non integer types\n");
        }
        if (!typecmp (right, integer_type)) {
                saerr ("Arithmetic on non integer types\n");
        }

        free (integer_type);
        return left;
}

Type* replace_op_assign (AST_Program* A, GUser_Types* G, Type* ret_type, Binary_Expr* B)
{
        Astn* new = malloc (sizeof (Astn));
        if (!new) {
                printf ("add assign replacement astn allocation fail\n");
                exit (EXIT_FAILURE);
        }

        new->kind = NODE_BINARY_EXPR;
        new->data.binary = malloc (sizeof (Binary_Expr));
        if (!new->data.binary) {
                printf ("add assign replacement binary expr allocation fail\n");
                exit (EXIT_FAILURE);
        }

        new->data.binary->right = B->right;
        new->data.binary->left  = astcp (B->left);

        if (B->op == TOK_ADD_ASSIGN) {
                new->data.binary->op = TOK_PLUS;
        } else if (B->op == TOK_SUB_ASSIGN) {
                new->data.binary->op = TOK_MINUS;
        } else if (B->op == TOK_MUL_ASSIGN) {
                new->data.binary->op = TOK_STAR;
        } else if (B->op == TOK_DIV_ASSIGN) {
                new->data.binary->op = TOK_SLASH;
        } else {
                // shouldnt be here
                saerr ("Token unmatched at replace op assign\n");
        }
        B->op = TOK_ASSIGN;
        // B->left stays the original lvalue (write target); B->right now
        // points at the "x + rhs" node just built (new->data.binary->left
        // holds an independent *copy* of the original x for that read), so
        // "x += rhs" becomes "x = x + rhs" rather than losing the "+ x".
        B->right = new;

        return binary_check (A, G, ret_type, B);
}

Type* assign_check (AST_Program* A, GUser_Types* G, Type* ret_type, Binary_Expr* B)
{
        if (B->left->kind != NODE_LITERAL || B->left->data.literal->kind != LIT_VAR) {
                saerr ("left-hand side of assignment must be a plain variable");
        }

        Type* left_type = ast_check (A, G, ret_type, B->left);

        if (left_type == NULL || left_type->kind != MUTABLE) {
                saerr ("cannot assign to an immutable variable");
        }

        Type* right_type = ast_check (A, G, ret_type, B->right);

        if (!typecmp (left_type, right_type)) {
                saerr ("assignment type mismatch");
        }

        return left_type;
}

Type* binary_check_help (AST_Program* A, GUser_Types* G, Type* ret_type, Binary_Expr* B)
{
        REQUIRES (B->op != TOK_ARROW_TYPE && B->op != TOK_DOT);

        if ((B->op == TOK_PLUS || B->op == TOK_MINUS) ||
            (B->op == TOK_STAR || B->op == TOK_SLASH) ||
            (B->op == TOK_EQ || B->op == TOK_LT) ||
            (B->op == TOK_LEQ || B->op == TOK_GT) ||
            (B->op == TOK_GEQ || B->op == TOK_AND) ||
            (B->op == TOK_OR)) {
                // Comparisons and logical and/or share arithmetic's "both
                // sides are int" check and also result in an int (this
                // language has no dedicated bool type; 0/1 double as
                // booleans, matching how loop/cond conditions are checked).
                return binary_check_int (A, G, ret_type, B);
        } else if (B->op == TOK_ASSIGN) {
                return assign_check (A, G, ret_type, B);
        } else if ((B->op == TOK_ADD_ASSIGN || B->op == TOK_SUB_ASSIGN) ||
                   (B->op == TOK_MUL_ASSIGN || B->op == TOK_DIV_ASSIGN)) {
                return replace_op_assign (A, G, ret_type, B);
        } else {
                printf ("binary experssion operator non matching");
                exit (EXIT_FAILURE);
        }

        return NULL; // should never reach here
}


Type* binary_check (AST_Program* A, GUser_Types* G, Type* ret_type, Binary_Expr* B)
{
        if (B->op == TOK_ARROW_TYPE) {
                B->op = TOK_DOT;
                Astn* left = B->left;
                Astn* new = malloc (sizeof (Astn));
                if (!new) {
                        printf ("Arrow type to dot Astn allocation fail\n");
                        exit (EXIT_FAILURE);
                }

                new->kind = NODE_UNARY_EXPR;
                new->data.unary = malloc (sizeof (Unary_Expr));
                if (!new->data.unary) {
                        printf ("Arrow type to dot unary allocation fail\n");
                        exit (EXIT_FAILURE);
                }

                new->data.unary->op = TOK_STAR;
                new->data.unary->arg = left;
                B->left = new;
                return binary_check (A, G, ret_type, B);
        } else if (B->op == TOK_DOT) {
                Type* user = ast_check (A, G, ret_type, B->left);
                
                if (user->kind != USER) {
                        printf ("Accessed non-struct");
                        exit (EXIT_FAILURE);
                }
                ASSERT (user->kind == USER);

                if (B->right->kind != NODE_LITERAL) {
                        printf ("Struct accessed by non literal");
                        exit (EXIT_FAILURE);
                }
                ASSERT (B->right->kind == NODE_LITERAL);

                if (B->right->data.literal->kind != LIT_VAR) {
                        printf ("Struct accessed by non variable\n");
                        exit (EXIT_FAILURE);
                }
                ASSERT (B->right->data.literal->kind == LIT_VAR);

                if (!isin_struct (user->data.user, B->right->data.literal->value.var)) {
                        printf ("Variable not in struct\n");
                        exit (EXIT_FAILURE);
                }

                return B->right->data.literal->type;
        } else {
                return binary_check_help (A, G, ret_type, B);
        }

        return NULL; // should never reach here
}

Type* unary_check (AST_Program* A, GUser_Types* G, Type* ret_type, Unary_Expr* U)
{
        if (U->op == TOK_RETURN) {
                Type* arg_type = ast_check (A, G, ret_type, U->arg);
                if (!ret_type) {
                        saerr ("return type invalid");
                }

                if (ret_type->kind == NONE) {
                        saerr ("none type function does not return");
                }

                if (!typecmp (ret_type, arg_type)) {
                        printf ("Incorrect return type\n");
                        exit (EXIT_FAILURE);
                }

                return arg_type;
        } else if (U->op == TOK_STAR) {
                return ast_check(A, G, ret_type, U->arg);
        } else if (U->op == TOK_MINUS) {
                Type* arg_type = ast_check (A, G, ret_type, U->arg);

                Type* integer_type = malloc (sizeof (Type));
                if (!integer_type) {
                        printf ("unary check negative integer type allocation failed\n");
                        exit (EXIT_FAILURE);
                }

                integer_type->kind = VALUE;
                integer_type->data.base = INT;

                if (!typecmp (arg_type, integer_type)) {
                        saerr ("unary check negative sign on non integer type");
                }
                
                free (integer_type);
                return arg_type;
        } else {
                saerr ("unary check invalid operator");
        }

        return NULL;
}

Type* fun_call_check (AST_Program* A, GUser_Types* G, Fun_Call* C)
{
        if (!isin_program_fun (A, C->fun_name)) {
                saerr ("Function call name not in program");
        }

        Fun_Type* F = program_get_fun (A, C->fun_name);

        Var_List* L = F->variables[0];

        if (L->num_var > C->num_arg) {
                saerr ("function call not enough args");
        } else if (L->num_var < C->num_arg) {
                saerr ("function call too many args");
        }

        size_t i = 0;
        size_t n = L->num_var;

        while (i < n) {
                Type* arg_type = ast_check (A, G, NULL, C->args[i]);
                Type* param_type = L->variables[i]->type;

                if (!typecmp (arg_type, param_type)) {
                        printf ("Function call arg %zu not matching function param", i);
                        saerr ("");
                }
                i++;
        }
        
        return F->ret_type;
}

void loop_check (AST_Program* A, GUser_Types* G, Type* ret_type, Loop_Expr* data)
{
        Type* cond_type = ast_check (A, G, NULL, data->cond);
        Type* integer_type = malloc (sizeof (Type));
        if (!integer_type) {
                printf ("loop check condition interger type allocation fail");
                exit (EXIT_FAILURE);
        }
        integer_type->kind = VALUE;
        integer_type->data.base = INT;

        if (!typecmp (cond_type, integer_type)) {
                saerr ("loop condition not int type");
        }

        free (integer_type);

        body_check (A, G, ret_type, data->body);
}

void cond_check (AST_Program* A, GUser_Types* G, Type* ret_type, Cond_Expr* data)
{
        // ELSE has no condition of its own (data->cond is NULL); only
        // IF/ELSEIF clauses need their condition type-checked.
        if (data->kind != ELSE) {
                Type* cond_type = ast_check (A, G, ret_type, data->cond);
                Type* integer_type = malloc (sizeof (Type));
                if (!integer_type) {
                        printf ("cond check condition integer type allocation fail");
                        exit (EXIT_FAILURE);
                }
                integer_type->kind = VALUE;
                integer_type->data.base = INT;

                if (!typecmp (cond_type, integer_type)) {
                        saerr ("cond condition not int type");
                }

                free (integer_type);
        }

        body_check (A, G, ret_type, data->body);

        if (data->chain != NULL) {
                cond_check (A, G, ret_type, data->chain);
        }
}

Type* ast_check (AST_Program* A, GUser_Types* G, Type* ret_type, Astn* ast)
{
        if (ast->kind == NODE_LITERAL) {
                return literal_check (ast->data.literal);
        } else if (ast->kind == NODE_BINARY_EXPR) {
                return binary_check (A, G, ret_type, ast->data.binary);
        } else if (ast->kind == NODE_UNARY_EXPR) {
                return unary_check (A, G, ret_type, ast->data.unary);
        } else if (ast->kind == NODE_FUN_CALL) {
                return fun_call_check (A, G, ast->data.fun_call);
        } else if (ast->kind == NODE_LAMBDA || ast->kind == NODE_LAMCALL) {
                saerr ("lambdas are not supported by this build (out of scope)");
        } else if (ast->kind == NODE_LOOP) {
                loop_check (A, G, ret_type, ast->data.loop);
        } else if (ast->kind == NODE_COND) {
                cond_check (A, G, ret_type, ast->data.cond);
        } else if (ast->kind == NODE_BODY) {
                body_check (A, G, ret_type, ast->data.body_block);
        } else if (ast->kind == NODE_FUN_DEC) {
                saerr ("Function declaration not allowed inside function");
        } else {
                saerr ("Semantic analysis ast kind not matched");
        }

        return NULL;
}

void body_check (AST_Program* A, GUser_Types* G, Type* ret_type, Body_Block* data)
{
        size_t i = 0;
        size_t n = data->num_inst;

        while (i < n) {
                ast_check (A, G, ret_type, data->inst[i++]);
        }
}

void fun_type_check (AST_Program* A, GUser_Types* G, Fun_Type* F)
{
        REQUIRES(is_fun(F));

        fun_name_check (A, F->fun_name);

        if (strcmp (F->fun_name, "main") == 0 &&
            (F->ret_type->kind != VALUE || F->ret_type->data.base != INT)) {
                saerr ("Main function return type is not int\n");
        }


        // body check, which will call the body checker
        body_check (A, G, F->ret_type, F->body);
}

/*
size_t alloc_size (AST_Program* A, GUser_Types* G, Astn* ast)
{
        // TODO
}
*/

void semantic_analysis (AST_Program *AP)
{
        size_t i = 0;
        size_t n = AP->function_count;
        GUser_Types* G = AP->G;

        // Typecheck
        #if DEBUG
        printf ("Typecheck Pass...\n");
        #endif
        while (i < n) {
                Astn* ast = AP->functions[i];
                if (ast->kind != NODE_FUN_DEC) {
                        saerr ("AST_Program functions list does not have function declaration");
                }
                fun_type_check (AP, G, ast->data.fun_dec);
                i++;
        }
        #if DEBUG
        printf ("Sucess!\n");
        #endif
        
        /*
        #if DEBUG
        printf ("Adding up allocation sizes...\n");
        #endif
        while (i < n) {
                Astn* ast = AP->functions[i];
                if (ast->kind != NODE_FUN_DEC) {
                        saerr ("AST_Program functions list does not have function declaration");
                }
                AP->size += alloc_size (AP, G, ast);
                i++;
        }
        #if DEBUG
        printf ("Sucess\n");
        #endif
        */
        

        // TODO: A final ast program checker
        // ENSURES (is_final_astprog (AP));
}

// ========================================================================= //
