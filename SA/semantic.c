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
#include <stdlib.h>
#include <stdbool.h>

#include <assert.h>

#include "../tokenizer/token.h"
#include "../parser/ast.h"
#include "../utils.h"

// ========================================================================= //


// ========================================================================= //
void semantic_analysis (AST_Program* A);

void ast_check (Astn* A);

bool is_type (Astn* A, Type* T);

// ========================================================================= //


// ========================================================================= //
bool is_type (Astn* A, Type* T)
{

}

void literal_check (Literal_Expr* data)
{
        // TODO
}

void binary_check (Binary_Expr* data)
{
        // TODO
}

void unary_check (Unary_Expr* data)
{
        // TODO
}

void fun_check (Fun_Type* data)
{
        // TODO
}

void call_check (Fun_Call* data)
{
        // TODO
}

void lambda_check (Fun_Type* data)
{
        // TODO
}

void lamcall_check (Fun_Type* data)
{
        // TODO
}

void loop_check (Fun_Type* data)
{
        // TODO
}

void cond_check (Fun_Type* data)
{
        // TODO
}

void body_check (Fun_Type* data)
{
        // TODO
}

void ast_check (Astn* A)
{
        Node_Kind k = A->kind;
        switch (k) {
                case NODE_LITERAL :
                        literal_check (A->data.literal);
                        break;
                case NODE_BINARY_EXPR :
                        binary_check (A->data.binary);
                        break;
                case NODE_UNARY_EXPR :
                        unary_check (A->data.unary);
                        break;
                case NODE_FUN_DEC :
                        fun_check (A->data.fun_dec);
                        break;
                case NODE_FUN_CALL :
                        call_check (A->data.fun_call);
                        break;
                case NODE_LAMBDA :
                        lambda_check (A->data.lambda);
                        break;
                case NODE_LAMCALL :
                        lamcall_check (A->data.lam_call);
                        break;
                case NODE_LOOP :
                        loop_check (A->data.loop);
                        break;
                case NODE_COND :
                        cond_check (A->data.cond);
                        break;
                case NODE_BODY :
                        body_check (A->data.body_block);
                        break;
                default :
                        // Every node should have a kind, it
                        // shouldnt reach here.
                        printf ("ast check kind did not match");
                        exit (EXIT_FAILURE);
        }
}

void semantic_analysis (AST_Program* AP)
{
        size_t i = 0;
        size_t n = AP->capacity;
        while (i < n) {
                ast_check (AP->functions[i++]);
        }
}


// ========================================================================= //

