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
// Every ast_comp call leaves exactly one value on the stack. body_comp is
// responsible for popping that value back off when a top-level statement's
// result is unused (everything except return, whose value is consumed by
// the return opcode itself).
//
// Forward jumps (if/goto) are compiled with a two-byte zero placeholder,
// then backpatched once the jump target address is known, via byte_index/
// byte_add_index/patch_jump below. This avoids having to thread byte-length
// counts through every compile function by hand.

void iaerr (char* msg); // defined in IR.c

void body_comp (IR_Program* I, AST_Program* A, IR* F,
                Vstack* S, uint8_t* total_vars, Body_Block* B);

void ast_comp (IR_Program* I, AST_Program* A, IR* F,
               Vstack* S, Astn* ast);

// ========================================================================= //
// Patch the two operand bytes of a jump instruction that starts at
// jump_start (the index of its opcode byte) so it targets `target`.
void patch_jump (IR* F, uint16_t jump_start, uint16_t target)
{
        int32_t offset = (int32_t) target - (int32_t) jump_start;
        uint8_t o1 = (uint8_t) ((offset >> 8) & 0x00FF);
        uint8_t o2 = (uint8_t) (offset & 0x00FF);
        byte_add_index (F, jump_start + 1, o1);
        byte_add_index (F, jump_start + 2, o2);
}

// Emits a 3-byte if/goto with a placeholder offset, returning the index of
// its opcode byte so the caller can patch_jump it later.
uint16_t emit_jump (IR* F, uint8_t opcode)
{
        uint16_t start = byte_index (F);
        byte_add (F, opcode);
        byte_add (F, 0);
        byte_add (F, 0);
        return start;
}

// ========================================================================= //
// When at a literal, we want to push it into the stack
void literal_comp (IR_Program* I, IR* F, Vstack* S, Literal_Expr* data)
{
        if (data->kind == LIT_INT) {
                uint16_t index = IR_add_int (I, data->value.int_val);
                byte_add (F, 0x13);
                byte_add (F, (uint8_t) ((index >> 8) & 0x00FF));
                byte_add (F, (uint8_t) (index & 0x00FF));
        } else if (data->kind == LIT_CHAR) {
                byte_add (F, 0x10);
                byte_add (F, (uint8_t) data->value.char_val);
        } else if (data->kind == LIT_VAR) {
                uint8_t index = search_vstack (S, data->value.var->name);
                byte_add (F, 0x15);
                byte_add (F, index);
        } else if (data->kind == LIT_NEW) {
                iaerr ("literal_comp: `new` allocation is not supported by this build (out of scope)");
        } else {
                iaerr ("literal_comp: unsupported literal kind");
        }
}

// x = expr;  ->  compile expr, dup it (so the assignment itself still
// yields a value like any other expression), vstore into x's slot.
void assign_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, Binary_Expr* data)
{
        if (data->left->kind != NODE_LITERAL || data->left->data.literal->kind != LIT_VAR) {
                iaerr ("assign_comp: left-hand side of assignment must be a plain variable (out of scope otherwise)");
        }

        ast_comp (I, A, F, S, data->right);

        byte_add (F, 0x59); // dup

        uint8_t index = search_vstack (S, data->left->data.literal->value.var->name);
        byte_add (F, 0x36); // vstore
        byte_add (F, index);
}

// left && right, short-circuited: if left is false, skip right entirely and
// the result is false; otherwise the result is right's value.
void and_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, Binary_Expr* data)
{
        ast_comp (I, A, F, S, data->left);

        uint16_t jmp_false = emit_jump (F, 0x9F); // if left == 0, skip right

        ast_comp (I, A, F, S, data->right);

        uint16_t jmp_end = emit_jump (F, 0xA7);

        uint16_t false_label = byte_index (F);
        byte_add (F, 0x10); // bipush 0
        byte_add (F, 0);

        uint16_t end_label = byte_index (F);

        patch_jump (F, jmp_false, false_label);
        patch_jump (F, jmp_end, end_label);
}

// left || right, short-circuited: if left is true, skip right and the
// result is true; otherwise the result is right's value.
void or_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, Binary_Expr* data)
{
        ast_comp (I, A, F, S, data->left);

        uint16_t jmp_eval_right = emit_jump (F, 0x9F); // if left == 0, go evaluate right

        byte_add (F, 0x10); // bipush 1 (left was true)
        byte_add (F, 1);

        uint16_t jmp_end = emit_jump (F, 0xA7);

        uint16_t eval_right_label = byte_index (F);
        ast_comp (I, A, F, S, data->right);

        uint16_t end_label = byte_index (F);

        patch_jump (F, jmp_eval_right, eval_right_label);
        patch_jump (F, jmp_end, end_label);
}

void binary_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, Binary_Expr* data)
{
        if (data->op == TOK_ASSIGN) {
                assign_comp (I, A, F, S, data);
                return;
        } else if (data->op == TOK_AND) {
                and_comp (I, A, F, S, data);
                return;
        } else if (data->op == TOK_OR) {
                or_comp (I, A, F, S, data);
                return;
        } else if (data->op == TOK_DOT || data->op == TOK_ARROW_TYPE) {
                iaerr ("binary_comp: struct field access is not supported by this build (out of scope)");
        }

        ast_comp (I, A, F, S, data->left);
        ast_comp (I, A, F, S, data->right);

        if (data->op == TOK_PLUS) {
                byte_add (F, 0x60);
        } else if (data->op == TOK_MINUS) {
                byte_add (F, 0x64);
        } else if (data->op == TOK_STAR) {
                byte_add (F, 0x68);
        } else if (data->op == TOK_SLASH) {
                byte_add (F, 0x6C);
        } else if (data->op == TOK_EQ) {
                byte_add (F, 0x61);
        } else if (data->op == TOK_LT) {
                byte_add (F, 0x62);
        } else if (data->op == TOK_LEQ) {
                byte_add (F, 0x63);
        } else if (data->op == TOK_GT) {
                byte_add (F, 0x65);
        } else if (data->op == TOK_GEQ) {
                byte_add (F, 0x66);
        } else {
                iaerr ("binary_comp: unsupported binary operator");
        }
}

void unary_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, Unary_Expr* data)
{
        if (data->op == TOK_RETURN) {
                ast_comp (I, A, F, S, data->arg);
                byte_add (F, 0xB0); // return
        } else if (data->op == TOK_MINUS) {
                ast_comp (I, A, F, S, data->arg);
                byte_add (F, 0x6F); // ineg
        } else if (data->op == TOK_STAR) {
                iaerr ("unary_comp: pointer dereference is not supported by this build (out of scope)");
        } else {
                iaerr ("unary_comp: unsupported unary operator");
        }
}

void funcall_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, Fun_Call* data)
{
        size_t i = 0;
        size_t n = data->num_arg;
        while (i < n) {
                ast_comp (I, A, F, S, data->args[i]);
                i++;
        }

        uint16_t index = search_fun (A, data->fun_name);
        byte_add (F, 0xB8);
        byte_add (F, (uint8_t) ((index >> 8) & 0x00FF));
        byte_add (F, (uint8_t) (index & 0x00FF));
}

void ast_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, Astn* ast)
{
        if (ast->kind == NODE_LITERAL) {
                literal_comp (I, F, S, ast->data.literal);
        } else if (ast->kind == NODE_BINARY_EXPR) {
                binary_comp (I, A, F, S, ast->data.binary);
        } else if (ast->kind == NODE_UNARY_EXPR) {
                unary_comp (I, A, F, S, ast->data.unary);
        } else if (ast->kind == NODE_FUN_CALL) {
                funcall_comp (I, A, F, S, ast->data.fun_call);
        } else if (ast->kind == NODE_LAMBDA || ast->kind == NODE_LAMCALL) {
                iaerr ("ast_comp: lambdas are not supported by this build (out of scope)");
        } else {
                iaerr ("ast_comp: unexpected ast node kind in expression position");
        }
}

void loop_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, uint8_t* total_vars, Loop_Expr* L)
{
        uint16_t cond_start = byte_index (F);
        ast_comp (I, A, F, S, L->cond);

        uint16_t jmp_exit = emit_jump (F, 0x9F); // if cond == 0, exit loop

        body_comp (I, A, F, S, total_vars, L->body);

        uint16_t jmp_back = emit_jump (F, 0xA7);
        patch_jump (F, jmp_back, cond_start);

        uint16_t exit_label = byte_index (F);
        patch_jump (F, jmp_exit, exit_label);
}

#define COND_CHAIN_MAX 64

// Compiles one IF/ELSEIF/ELSE chain node and recurses down ->chain,
// collecting the "jump past the rest of the chain" placeholders taken by
// each true branch so the caller can patch them all to the chain's end once
// that address is known.
void cond_chain_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, uint8_t* total_vars,
                      Cond_Expr* C, uint16_t* end_jumps, size_t* n_end_jumps)
{
        if (C == NULL) {
                return;
        }

        if (C->kind == ELSE) {
                body_comp (I, A, F, S, total_vars, C->body);
                return;
        }

        ast_comp (I, A, F, S, C->cond);

        uint16_t jmp_next = emit_jump (F, 0x9F); // if cond == 0, try next clause

        body_comp (I, A, F, S, total_vars, C->body);

        if (C->chain != NULL) {
                if (*n_end_jumps >= COND_CHAIN_MAX) {
                        iaerr ("cond_chain_comp: if/elseif chain too long");
                }
                end_jumps[(*n_end_jumps)++] = emit_jump (F, 0xA7);
        }

        uint16_t next_label = byte_index (F);
        patch_jump (F, jmp_next, next_label);

        cond_chain_comp (I, A, F, S, total_vars, C->chain, end_jumps, n_end_jumps);
}

void cond_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, uint8_t* total_vars, Cond_Expr* C)
{
        uint16_t end_jumps[COND_CHAIN_MAX];
        size_t n_end_jumps = 0;

        cond_chain_comp (I, A, F, S, total_vars, C, end_jumps, &n_end_jumps);

        uint16_t end_label = byte_index (F);
        size_t i = 0;
        while (i < n_end_jumps) {
                patch_jump (F, end_jumps[i], end_label);
                i++;
        }
}

void body_comp (IR_Program* I, AST_Program* A, IR* F, Vstack* S, uint8_t* total_vars, Body_Block* B)
{
        *total_vars += (uint8_t) B->vars->num_var;

        push_vstack (S, B->vars);

        // Declared-variable initializers run in declaration order, before
        // the body's own instructions, matching the [ ... ] / { ... } split.
        size_t d = 0;
        while (d < B->vars->num_var) {
                Var* v = B->vars->variables[d];
                if (v->value != NULL) {
                        ast_comp (I, A, F, S, v->value);
                        uint8_t index = search_vstack (S, v->name);
                        byte_add (F, 0x36); // vstore
                        byte_add (F, index);
                }
                d++;
        }

        size_t i = 0;
        size_t n = B->num_inst;

        while (i < n) {
                Astn* ast = B->inst[i++];

                if (ast->kind == NODE_BODY) {
                        body_comp (I, A, F, S, total_vars, ast->data.body_block);
                } else if (ast->kind == NODE_LOOP) {
                        loop_comp (I, A, F, S, total_vars, ast->data.loop);
                } else if (ast->kind == NODE_COND) {
                        cond_comp (I, A, F, S, total_vars, ast->data.cond);
                } else if (ast->kind == NODE_UNARY_EXPR && ast->data.unary->op == TOK_RETURN) {
                        // return transfers control away; nothing to pop.
                        unary_comp (I, A, F, S, ast->data.unary);
                } else {
                        ast_comp (I, A, F, S, ast);
                        byte_add (F, 0x57); // pop: discard unused statement result
                }
        }

        pop_vstack (S);
}

void fun_comp (IR_Program* I, AST_Program* A, Fun_Type* F)
{
        IR* fun = new_IR ();

        Var_List* V = F->variables[0];
        fun->num_args = (uint8_t) V->num_var;

        Vstack* S = new_vstack ();
        push_vstack (S, V);

        uint8_t total_vars = 0;

        body_comp (I, A, fun, S, &total_vars, F->body);

        fun->num_vars = total_vars;

        IR_add_fun (I, fun);
}

IR_Program* IR_Comp (AST_Program* A)
{
        if (A->functions[0] == NULL) {
                iaerr ("IR_Comp: program has no main function");
        }

        IR_Program* I = new_IR_Program ();

        size_t i = 0;
        size_t n = A->function_count;

        while (i < n) {
                Astn* ast = A->functions[i++];
                if (ast->kind != NODE_FUN_DEC) {
                        iaerr ("IR_Comp: AST_Program functions list contains a non-function-declaration node");
                }

                fun_comp (I, A, ast->data.fun_dec);
        }

        return I;
}

// ========================================================================= //
