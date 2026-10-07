// ========================================================================= //
// Andy Kwak 2026

// My Intermediate Representation compiler implementation
// AST_Program* -> IR_Program*

// Supported for now: int/char values, local variables, arithmetic,
// comparisons, and/or, assignment, if/elseif/else, while, function calls
// (including recursion), return. Heap (new, pointers, structs) and lambdas
// are rejected with an error.

// ========================================================================= //


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include "../tokenizer/token.h"

#include "../utils.h"

#include "../parser/ast.h"

#include "IR.h"
#include "IRComp.h"

// ========================================================================= //
// Everything needed while lowering one function
typedef struct Comp {
        IR_Program* I;
        AST_Program* A;
        Fun_Type* fun;
        IR* F;
        Vstack* S;
} Comp;

void expr_comp (Comp* C, Astn* ast);

void stmt_comp (Comp* C, Astn* ast);

void body_comp (Comp* C, Body_Block* B);

// ========================================================================= //
// Error reporting

void irerr (Comp* C, char* msg)
{
        fprintf (stderr, "IR error in function '%s': %s\n",
                 C->fun->fun_name, msg);
        exit (EXIT_FAILURE);
}

void irerr_name (Comp* C, char* msg, char* name)
{
        fprintf (stderr, "IR error in function '%s': %s '%s'\n",
                 C->fun->fun_name, msg, name);
        exit (EXIT_FAILURE);
}

// ========================================================================= //
// Emit helpers

void emit_u16 (IR* F, uint16_t x)
{
        byte_add (F, (uint8_t) ((x >> 8) & 0x00FF));
        byte_add (F, (uint8_t) (x & 0x00FF));
}

// Emits a jump with a placeholder offset, returns the jump's address so it
// can be patched once the target is known
uint16_t emit_jump (IR* F, uint8_t op)
{
        uint16_t at = byte_index (F);
        byte_add (F, op);
        emit_u16 (F, 0);
        return at;
}

void patch_jump (Comp* C, uint16_t at, uint16_t target)
{
        int32_t offset = (int32_t) target - (int32_t) at;
        if (offset < INT16_MIN || offset > INT16_MAX) {
                irerr (C, "jump too far for 16 bit offset");
        }
        uint16_t o = (uint16_t) (int16_t) offset;
        byte_add_index (C->F, at + 1, (uint8_t) ((o >> 8) & 0x00FF));
        byte_add_index (C->F, at + 2, (uint8_t) (o & 0x00FF));
}

// Jumps to an already known (backwards) target
void emit_jump_to (Comp* C, uint8_t op, uint16_t target)
{
        uint16_t at = emit_jump (C->F, op);
        patch_jump (C, at, target);
}

void emit_int (Comp* C, int32_t x)
{
        if (x >= INT8_MIN && x <= INT8_MAX) {
                byte_add (C->F, OP_BIPUSH);
                byte_add (C->F, (uint8_t) (int8_t) x);
        } else {
                byte_add (C->F, OP_ILDC);
                emit_u16 (C->F, IR_add_int (C->I, x));
        }
}

// ========================================================================= //
// Variables

uint8_t var_slot (Comp* C, Var* var, Var** decl)
{
        int slot = search_vstack (C->S, var->name, decl);
        if (slot < 0) {
                irerr_name (C, "undeclared variable", var->name);
        }
        return (uint8_t) slot;
}

void unsupported_type (Comp* C, Var* var)
{
        Type* t = var->type;
        if (t != NULL && t->kind == MUTABLE) {
                t = t->data.mutable;
        }
        if (t == NULL) {
                return;
        }
        if (t->kind == FUNCTION) {
                irerr_name (C, "function typed variables are not supported yet:",
                            var->name);
        } else if (t->kind == POINTER || t->kind == USER || t->kind == LIST) {
                irerr_name (C, "pointer/struct/list variables are not supported yet:",
                            var->name);
        }
}

// ========================================================================= //
// Expressions: each leaves exactly one value on the operand stack

bool is_assign_op (TokenType op)
{
        return op == TOK_ASSIGN || op == TOK_ADD_ASSIGN || op == TOK_SUB_ASSIGN ||
               op == TOK_MUL_ASSIGN || op == TOK_DIV_ASSIGN;
}

uint8_t arith_op (TokenType op)
{
        switch (op) {
                case TOK_PLUS :
                case TOK_ADD_ASSIGN :
                        return OP_IADD;
                case TOK_MINUS :
                case TOK_SUB_ASSIGN :
                        return OP_ISUB;
                case TOK_STAR :
                case TOK_MUL_ASSIGN :
                        return OP_IMUL;
                case TOK_SLASH :
                case TOK_DIV_ASSIGN :
                        return OP_IDIV;
                case TOK_EQ :
                        return OP_ICMPEQ;
                case TOK_LT :
                        return OP_ICMPLT;
                case TOK_LEQ :
                        return OP_ICMPLE;
                case TOK_GT :
                        return OP_ICMPGT;
                case TOK_GEQ :
                        return OP_ICMPGE;
                default :
                        return OP_NOP;
        }
}

void literal_comp (Comp* C, Literal_Expr* L)
{
        if (L->kind == LIT_INT) {
                emit_int (C, L->value.int_val);
        } else if (L->kind == LIT_CHAR) {
                emit_int (C, L->value.char_val);
        } else if (L->kind == LIT_VAR) {
                uint8_t slot = var_slot (C, L->value.var, NULL);
                byte_add (C->F, OP_VLOAD);
                byte_add (C->F, slot);
        } else {
                irerr (C, "'new' heap allocation is not supported yet");
        }
}

// Stores the result of a (compound) assignment, leaves nothing on the stack
uint8_t assign_comp (Comp* C, Binary_Expr* B)
{
        if (B->left->kind != NODE_LITERAL ||
            B->left->data.literal->kind != LIT_VAR) {
                irerr (C, "can only assign to a variable for now");
        }

        Var* decl = NULL;
        uint8_t slot = var_slot (C, B->left->data.literal->value.var, &decl);

        if (decl->type == NULL || decl->type->kind != MUTABLE) {
                irerr_name (C, "cannot assign to immutable variable (declare it with $):",
                            decl->name);
        }

        if (B->op != TOK_ASSIGN) {
                byte_add (C->F, OP_VLOAD);
                byte_add (C->F, slot);
                expr_comp (C, B->right);
                byte_add (C->F, arith_op (B->op));
        } else {
                expr_comp (C, B->right);
        }

        byte_add (C->F, OP_VSTORE);
        byte_add (C->F, slot);

        return slot;
}

// a and b, a or b, with short circuiting. Leaves 1 or 0.
void logic_comp (Comp* C, Binary_Expr* B)
{
        IR* F = C->F;

        expr_comp (C, B->left);
        if (B->op == TOK_AND) {
                // if left is false, the answer is 0
                uint16_t j_false = emit_jump (F, OP_IF);
                expr_comp (C, B->right);
                uint16_t j_false2 = emit_jump (F, OP_IF);
                emit_int (C, 1);
                uint16_t j_end = emit_jump (F, OP_GOTO);
                patch_jump (C, j_false, byte_index (F));
                patch_jump (C, j_false2, byte_index (F));
                emit_int (C, 0);
                patch_jump (C, j_end, byte_index (F));
        } else {
                // if left is false, the answer depends on right
                uint16_t j_right = emit_jump (F, OP_IF);
                emit_int (C, 1);
                uint16_t j_end = emit_jump (F, OP_GOTO);
                patch_jump (C, j_right, byte_index (F));
                expr_comp (C, B->right);
                uint16_t j_false = emit_jump (F, OP_IF);
                emit_int (C, 1);
                uint16_t j_end2 = emit_jump (F, OP_GOTO);
                patch_jump (C, j_false, byte_index (F));
                emit_int (C, 0);
                patch_jump (C, j_end, byte_index (F));
                patch_jump (C, j_end2, byte_index (F));
        }
}

void binary_comp (Comp* C, Binary_Expr* B)
{
        if (is_assign_op (B->op)) {
                // assignment used as a value, e.g. x = y = 3
                uint8_t slot = assign_comp (C, B);
                byte_add (C->F, OP_VLOAD);
                byte_add (C->F, slot);
                return;
        }

        if (B->op == TOK_AND || B->op == TOK_OR) {
                logic_comp (C, B);
                return;
        }

        uint8_t op = arith_op (B->op);
        if (op == OP_NOP) {
                irerr (C, "struct/pointer member access is not supported yet");
        }

        expr_comp (C, B->left);
        expr_comp (C, B->right);
        byte_add (C->F, op);
}

void unary_comp (Comp* C, Unary_Expr* U)
{
        if (U->op == TOK_MINUS) {
                expr_comp (C, U->arg);
                byte_add (C->F, OP_INEG);
        } else if (U->op == TOK_STAR) {
                irerr (C, "pointer dereference is not supported yet");
        } else if (U->op == TOK_RETURN) {
                irerr (C, "return cannot be used as a value");
        } else {
                irerr (C, "unknown unary operator");
        }
}

void funcall_comp (Comp* C, Fun_Call* call)
{
        int index = search_fun (C->A, call->fun_name);
        if (index < 0) {
                irerr_name (C, "call to undeclared function", call->fun_name);
        }

        Fun_Type* callee = C->A->functions[index]->data.fun_dec;
        if (callee->variables[0]->num_var != call->num_arg) {
                irerr_name (C, "wrong number of arguments in call to",
                            call->fun_name);
        }

        size_t i = 0;
        while (i < call->num_arg) {
                expr_comp (C, call->args[i++]);
        }

        byte_add (C->F, OP_INVOKESTATIC);
        emit_u16 (C->F, (uint16_t) index);
}

void expr_comp (Comp* C, Astn* ast)
{
        switch (ast->kind) {
                case NODE_LITERAL :
                        literal_comp (C, ast->data.literal);
                        break;
                case NODE_BINARY_EXPR :
                        binary_comp (C, ast->data.binary);
                        break;
                case NODE_UNARY_EXPR :
                        unary_comp (C, ast->data.unary);
                        break;
                case NODE_FUN_CALL :
                        funcall_comp (C, ast->data.fun_call);
                        break;
                case NODE_LAMCALL : {
                        // the parser only knows functions declared above it,
                        // so a call to a later function shows up as a lambda call
                        Astn* callee = ast->data.lam_call->function;
                        if (callee->kind == NODE_LITERAL &&
                            callee->data.literal->kind == LIT_VAR &&
                            search_fun (C->A, callee->data.literal->value.var->name) >= 0) {
                                irerr_name (C, "function must be declared before it is called:",
                                            callee->data.literal->value.var->name);
                        }
                        irerr (C, "lambdas are not supported yet");
                        break;
                }
                case NODE_LAMBDA :
                        irerr (C, "lambdas are not supported yet");
                        break;
                default :
                        irerr (C, "expected an expression");
        }
}

// ========================================================================= //
// Statements: leave the operand stack as they found it

void loop_comp (Comp* C, Loop_Expr* L)
{
        IR* F = C->F;

        uint16_t top = byte_index (F);
        expr_comp (C, L->cond);
        uint16_t j_exit = emit_jump (F, OP_IF);

        body_comp (C, L->body);

        emit_jump_to (C, OP_GOTO, top);
        patch_jump (C, j_exit, byte_index (F));
}

void cond_comp (Comp* C, Cond_Expr* link)
{
        IR* F = C->F;

        if (link->kind == ELSE) {
                body_comp (C, link->body);
                return;
        }

        expr_comp (C, link->cond);
        uint16_t j_next = emit_jump (F, OP_IF);

        body_comp (C, link->body);

        if (link->chain != NULL) {
                uint16_t j_end = emit_jump (F, OP_GOTO);
                patch_jump (C, j_next, byte_index (F));
                cond_comp (C, link->chain);
                patch_jump (C, j_end, byte_index (F));
        } else {
                patch_jump (C, j_next, byte_index (F));
        }
}

void stmt_comp (Comp* C, Astn* ast)
{
        switch (ast->kind) {
                case NODE_BODY :
                        body_comp (C, ast->data.body_block);
                        return;
                case NODE_LOOP :
                        loop_comp (C, ast->data.loop);
                        return;
                case NODE_COND :
                        cond_comp (C, ast->data.cond);
                        return;
                case NODE_UNARY_EXPR :
                        if (ast->data.unary->op == TOK_RETURN) {
                                expr_comp (C, ast->data.unary->arg);
                                byte_add (C->F, OP_RETURN);
                                return;
                        }
                        break;
                case NODE_BINARY_EXPR :
                        if (is_assign_op (ast->data.binary->op)) {
                                assign_comp (C, ast->data.binary);
                                return;
                        }
                        break;
                default :
                        break;
        }

        // expression statement, throw away its value
        expr_comp (C, ast);
        byte_add (C->F, OP_POP);
}

void body_comp (Comp* C, Body_Block* B)
{
        push_vstack (C->S, B->vars);

        // initialize declared variables in order, uninitialized ones get 0
        if (B->vars != NULL) {
                size_t i = 0;
                while (i < B->vars->num_var) {
                        Var* var = B->vars->variables[i];
                        unsupported_type (C, var);

                        if (var->value != NULL) {
                                expr_comp (C, var->value);
                        } else {
                                emit_int (C, 0);
                        }
                        byte_add (C->F, OP_VSTORE);
                        byte_add (C->F, C->S->stack[C->S->num - 1].base + (uint8_t) i);
                        i++;
                }
        }

        size_t i = 0;
        while (i < B->num_inst) {
                stmt_comp (C, B->inst[i++]);
        }

        pop_vstack (C->S);
}

// ========================================================================= //
// Functions

IR* fun_comp (IR_Program* I, AST_Program* A, Fun_Type* fun)
{
        Comp C;
        C.I = I;
        C.A = A;
        C.fun = fun;
        C.F = new_IR (fun->fun_name);
        C.S = new_vstack ();

        Var_List* params = fun->variables[0];
        if (params->num_var > UINT8_MAX) {
                irerr (&C, "too many parameters (max 255)");
        }

        size_t i = 0;
        while (i < params->num_var) {
                unsupported_type (&C, params->variables[i++]);
        }

        // parameters take slots 0 .. num_args - 1
        push_vstack (C.S, params);

        body_comp (&C, fun->body);

        // falling off the end of a function returns 0
        emit_int (&C, 0);
        byte_add (C.F, OP_RETURN);

        pop_vstack (C.S);

        C.F->num_args = (uint8_t) params->num_var;
        C.F->num_vars = C.S->max;

        vstack_free (C.S);

        return C.F;
}

IR_Program* IR_Comp (AST_Program* A)
{
        if (A->function_count == 0 || A->functions[0] == NULL) {
                fprintf (stderr, "IR error: program has no main function\n");
                exit (EXIT_FAILURE);
        }

        IR_Program* I = new_IR_Program ();

        // function i in the AST is function i in the pool, main is 0
        size_t i = 0;
        size_t n = A->function_count;

        while (i < n) {
                Astn* ast = A->functions[i++];
                if (ast->kind != NODE_FUN_DEC) {
                        fprintf (stderr, "AST_Program functions contains non function ast\n");
                        exit (EXIT_FAILURE);
                }

                IR_add_fun (I, fun_comp (I, A, ast->data.fun_dec));
        }

        return I;
}

// ========================================================================= //
