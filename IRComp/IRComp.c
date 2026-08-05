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

uint8_t body_comp (IR_Program* I, AST_Program* A, IR* F, 
                   Vstack* S, uint8_t* na, Body_Block* B);

uint16_t ast_comp (IR_Program* I, AST_Program* A, IR* F, 
                   Vstack* S, uint8_t* na, Astn* ast);

// ========================================================================= //
// When at a literal, we want to push it into the stack
uint16_t literal_comp (IR_Program* I, IR* F, 
                       Vstack* S, uint8_t* na, Literal_Expr* data)
{
        uint16_t ret = 0;
        if (data->kind == LIT_INT) {
                uint16_t index = IR_add_int (I, data->value.int_val);
                uint8_t c1 = (uint8_t) ((index >> 8) & 0x00FF);
                uint8_t c2 = (uint8_t) (index & 0x00FF);
                byte_add (F, 0x13);
                byte_add (F, c1);
                byte_add (F, c2);
                ret += 3;
        } else if (data->kind == LIT_CHAR) {
                byte_add (F, 0x10);
                byte_add (F, (uint8_t) data->value.char_val);
                ret += 2;
        } else if (data->kind == LIT_VAR) {
                uint8_t index = search_vstack (S, data->value.var->name);
                byte_add (F, 0x15);
                byte_add (F, index);
                ret += 2;
        }

        return ret;
}

uint16_t binary_comp (IR_Program* I, AST_Program* A, IR* F, 
                      Vstack* S, uint8_t* na, Binary_Expr* data)
{
        uint16_t ret = 0;
        ret += ast_comp (I, A, F, S, data->left);
        ret += ast_comp (I, A, F, S, data->right);

        if (data->op == TOK_PLUS) {
                byte_add (F, 0x60);
        } else if (data->op == TOK_MINUS) {
                byte_add (F, 0x64);
        } else if (data->op == TOK_STAR) {
                byte_add (F, 0x68);
        } else if (data->op == TOK_SLASH) {
                byte_add (F, 0x6C);
        } else if (data->op == TOK_DOT) {
                // TODO
        }

        ret += 1;

        return ret;
}

uint16_t unary_comp (IR_Program* I, AST_Program* A, IR* F, 
                     Vstack* S, uint8_t* na, Unary_Expr* data)
{
        uint16_t ret = ast_comp (I, A, F, S, data->arg);

        if (data->op == TOK_STAR) {
                byte_add (F, 0x2E);
        } else if (data->op == TOK_MINUS) {
                byte_add (F, 0x6F);
        }
        ret += 1;

        return ret;
}

uint16_t funcall_comp (IR_Program* I, AST_Program* A, IR* F, 
                       Vstack* S, uint8_t* na, Fun_Call* data)
{
        size_t i = 0;
        size_t n = data->num_arg;
        uint16_t ret = 0;
        while (i < n) {
                ret += ast_comp (I, A, F, S, data->args[i++]);
        }

        byte_add (F, 0xB8);
        uint16_t index = search_fun (A, data->fun_name);
        uint8_t c1 = (uint8_t) ((index >> 8) & 0x00FF);
        uint8_t c2 = (uint8_t) (index & 0x00FF);

        byte_add (F, c1);
        byte_add (F, c2);
        ret += 3;

        return ret;
}

uint16_t ast_comp (IR_Program* I, AST_Program* A, IR* F, 
                   Vstack* S, uint8_t* na, Astn* ast)
{
        if (ast->kind == NODE_LITERAL) {
                return literal_comp (I, F, S, na, ast->data.literal);
        } else if (ast->kind == NODE_BINARY_EXPR) {
                return binary_comp (I, A, F, S, na, ast->data.binary);
        } else if (ast->kind == NODE_UNARY_EXPR) {
                return unary_comp (I, A, F, S, na, ast->data.unary);
        } else if (ast->kind == NODE_FUN_CALL) {
                return funcall_comp (I, A, F, S, na, ast->data.fun_call);
        }
}

uint16_t loop_comp (IR_Program* I, AST_Program* A, IR* F, 
                    Vstack* S, uint8_t* na, Loop_Expr* L)
{
        uint16_t ret = 0;
        ret += ast_comp (I, A, F, S, L->cond);

        byte_add (F, 0x9F);
        uint16_t i1 = byte_index (F);
        uint16_t i2 = byte_index (F) + 1;

        uint16_t jump = body_comp (I, A, F, S, na, L->body);

        ret += jump;

        uint8_t o1 = (uint8_t) ((jump >> 8) & 0x00FF);
        uint8_t o2 = (uint8_t) (jump & 0x00FF);
        byte_add (F, o1);
        byte_add (F, o2);

        ret += 3;

        byte_add (F, 0xA7);
        o1 = (uint8_t) ((ret >> 8) & 0x00FF);
        o2 = (uint8_t) (ret & 0x00FF);
        byte_add (F, o1);
        byte_add (F, o2);

        ret += 3;

        return ret;
}

uint16_t cond_comp (IR_Program* I, AST_Program* A, IR* F, 
                    Vstack* S, uint8_t* na, Cond_Expr* C)
{
        // TODO
}

uint16_t body_comp (IR_Program* I, AST_Program* A, IR* F, 
                   Vstack* S, uint8_t* na, Body_Block* B)
{
        uint16_t ret = 0;

        *na += B->vars->num_var;

        size_t i = 0;
        size_t n = B->num_inst;

        push_vstack (S, B->vars);

        while (i < n) {
                Astn* ast = B->inst[i++];

                if (ast->kind == NODE_BODY) {
                        ret += body_comp (I, A, F, S, na, ast->data.body_block);
                } else if (ast->kind == NODE_LOOP) {
                        ret += loop_comp (I, A, F, S, na, ast->data.loop);
                } else if (ast->kind == NODE_COND) {
                        ret += cond_comp (I, A, F, S, na, ast->data.cond);
                } else {
                        ret += ast_comp (I, A, F, S, na, ast);
                }
        }

        pop_vstack (S);

        return ret;
}

void fun_comp (IR_Program* I, AST_Program* A, Fun_Type* F)
{
        IR* fun = new_IR ();

        Var_List* V = F->variables[0];

        fun->num_args = (uint8_t) V->num_var;

        Vstack* S = new_vstack ();

        push_vstack (S, F->variables[0]);

        uint8_t* na = calloc (sizeof (uint8_t), 1);

        body_comp (I, A, fun, S, na, F->body);

        fun->num_args = na;
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
