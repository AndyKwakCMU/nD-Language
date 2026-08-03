// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation compiler implementation
// AST_Program* -> IR_Program*

// ========================================================================= //


#include <stdlib.h>
#include <stdint.h>


#include "../tokenizer/token.h"

#include "../utils.h"

#include "../parser/ast.h"

#include "IR.h"
#include "IRComp.h"

// ========================================================================= //
IR_Program* IR_Comp (AST_Program* A);

uint8_t body_comp (IR_Program* I, AST_Program* A, IR* F, Body_Block* B);

void ast_comp (IR_Program* I, AST_Program* A, IR* F, Astn* ast);

// ========================================================================= //
// When at a literal, we want to push it into the stack
void literal_comp (IR_Program* I, AST_Program* A, IR* F, Literal_Expr* data)
{
        if (data->kind == LIT_INT) {
                uint16_t index = IR_add_int (I, data->value.int_val);
                uint8_t c1 = (index >> 8) & 0x00FF;
                uint8_t c2 = index & 0x00FF;
                byte_add (F, 0x13);
                byte_add (F, c1);
                byte_add (F, c2);
        } else if (data->kind == LIT_CHAR) {
                byte_add (F, 0x10);
                byte_add (F, (uint8_t) data->value.char_val)
        } else if (data->kind == LIT_VAR) {
                uint8_t index = 
        }
}

void binary_comp (IR_Program* I, AST_Program* A, IR* F, Binary_Expr* data)
{

}

void unary_comp (IR_Program* I, AST_Program* A, IR* F, Unary_Expr* data)
{

}

void funcall_comp (IR_Program* I, AST_Program* A, IR* F, Fun_Call* data)
{
        // TODO
}

void ast_comp (IR_Program* I, AST_Program* A, IR* F, Astn* ast)
{
        if (ast->kind == NODE_LITERAL) {
                literal_comp (I, A, F, ast->data.literal);
        } else if (ast->kind == NODE_BINARY_EXPR) {
                binary_comp (I, A, F, ast->data.binary);
        } else if (ast->kind == NODE_UNARY_EXPR) {
                unary_comp (I, A, F, ast->data.unary);
        } else if (ast->kind == NODE_FUN_CALL) {
                funcall_comp (I, A, F, ast->data.fun_call);
        }
}

uint8_t loop_comp (IR_Program* I, AST_Program* A, IR* F, Loop_Expr* L)
{
        // TODO
}

uint8_t cond_comp (IR_Program* I, AST_Program* A, IR* F, Cond_Expr* C)
{
        // TODO
}

uint8_t body_comp (IR_Program* I, AST_Program* A, IR* F, Body_Block* B)
{
        uint8_t vars = B->vars->num_var;

        size_t i = 0;
        size_t n = B->num_inst;

        while (i < n) {
                Astn* ast = B->inst[i++];

                if (ast->kind == NODE_BODY) {
                        vars += body_comp (I, A, F, ast->data.body_block);
                } else if (ast->kind == NODE_LOOP) {
                        vars += loop_comp (I, A, F, ast->data.loop);
                } else if (ast->kind == NODE_COND) {
                        vars += cond_comp (I, A, F, ast->data.cond);
                } else {
                        ast_comp (I, A, F, ast);
                }
        }


        return vars;
}

void fun_comp (IR_Program* I, AST_Program* A, Fun_Type* F)
{
        IR* fun = new_IR ();

        Var_List* V = F->variables[0];

        fun->num_args = (uint8_t) V->num_var;

        fun->num_args = body_comp (I, A, fun, F->body);

        IR_add_fun (I, fun);
}

IR_Program* IR_Comp (AST_Program* A)
{
        IR_Program* I = new_IR_Program ();
        
        size_t i = 0;
        size_t n = A->function_count;

        while (i < n) {
                Astn* ast = A->functions[i++];
                if (ast->kind != NODE_FUN_DEC) {
                        printf ("AST_Program functions contains non function ast\n");
                        exit (EXIT_FAILURE);
                }

                Fun_Type* fun = ast->data.fun_dec;
                fun_comp (I, A, fun);
        }

        return I;
}

// ========================================================================= //
