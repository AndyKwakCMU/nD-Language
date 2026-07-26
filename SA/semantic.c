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
#include <stdlib.h>

#include <assert.h>

#include "../parser/ast.h"
#include "../tokenizer/token.h"
#include "../utils.h"

// ========================================================================= //

// ========================================================================= //
void semantic_analysis(AST_Program *A);

void ast_check(Astn *A);

bool is_type(Astn *A, Type *T);

// ========================================================================= //

// ========================================================================= //
bool is_type(Astn *A, Type *T) {
        // TODO
}

void literal_check(Literal_Expr *data) {
        // TODO
}

void binary_check(Binary_Expr *data) {
        // TODO
}

void unary_check(Unary_Expr *data) {
        // TODO
}

void call_check(Fun_Call *data) {
        // TODO
}

void lambda_check(Fun_Type *data) {
        // TODO
}

void lamcall_check(Fun_Type *data) {
        // TODO
}

void loop_check(Fun_Type *data) {
        // TODO
}

void cond_check(Fun_Type *data) {
        // TODO
}

void body_check(Fun_Type *data) {
        // TODO
}

bool is_fun(Fun_Type *F) {
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
void fun_name_check(AST_Program *A, char *name) {
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
                printf("Too many functions of the name %s is in AST_Program", name);
                saerr("");
        } else if (count < 1) {
                printf("No functions of the name %s is in AST_Program", name);
                saerr("");
        } // else (count == 1) do nothing
}

void fun_check(AST_Program *A, GUser_Types *G, Fun_Type *F) {
        REQUIRES(is_fun(F));

        fun_name_check(A, F->fun_name);

        // variable list check....hmmm what happened to variable lists that were out
        // of scope? did they remain in the body variable list? how do we add them all
        // up for a singular function arena allocation

        // body check, which will call the body checker
}

void semantic_analysis(AST_Program *AP) {
        size_t i = 0;
        size_t n = AP->capacity;
        GUser_Types *G = AP->G;
        while (i < n) {
                Astn *ast = AP->functions[i];
                if (ast->kind != NODE_FUN_DEC) {
                        saerr("AST_Program functions list does not have function declaration");
                }

                Fun_Type *F = ast->data.fun_dec;
                fun_check(AP, G, F);
                i++;
        }

        // TODO: A final ast program checker
        // ENSURES (is_final_astprog (AP));
}

// ========================================================================= //
