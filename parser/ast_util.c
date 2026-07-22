// ========================================================================= //
// Andy Kwak 2026

// Printing the AST Program

// ========================================================================= //

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "../utils.h"

// ========================================================================= //
void print_ast (Astn* A);

void print_body (Body_Block* B);

void print_type (Type* type);

void print_varlist (Var_List* V);

void print_var (Var* var);

// ========================================================================= //
void print_type (Type* type)
{
        if (!type) {
                printf ("No Type\n");
        } else if (type->kind == VALUE) {
                switch (type->data.base) {
                        case INT :
                                printf ("int\n");
                                break;

                        case CHAR :
                                printf ("char\n");
                                break;
                        default :
                                printf ("kind not matching with data\n");
                                break;
                }
        } else if (type->kind == POINTER) {
                printf ("Pointer to: \n");
                print_type (type->data.pointer);
        } else if (type->kind == FUNCTION) {
                printf ("left:\n");
                print_type (type->data.tree->input);
                printf ("right:\n");
                print_type (type->data.tree->output);
        } else if (type->kind == MUTABLE) {
                printf ("Mutable: \n");
                print_type (type->data.mutable);
        } else { // USER
                printf ("User struct: %s\n", type->data.user->name);
                if (type->data.user->kind == STRUCT) {
                        printf ("struct:\n");
                        print_varlist (type->data.user->data.Struct);
                } else {
                        printf ("Alias:\n");
                        print_type (type->data.user->data.Alias);
                }
        }
        
}

void print_var (Var* var)
{
        if (!var) {
                printf ("No var\n");
        } else {
                printf ("Variable name: '%s', Type: \n", var->name);
                print_type (var->type);
                printf ("Value: \n");
                print_ast  (var->value);
        }
}

void print_param (Var_List* params)
{
        if (!params) {
                perror ("parameter list uninitialized");
                exit (EXIT_FAILURE);
        }

        size_t i = 0;
        while (i < params->num_var) {
                printf ("Param #%zu name: '%s', Type: \n", i, params->variables[i]->name);
                print_type (params->variables[i]->type);
                i++;
        }
        
}

void print_varlist (Var_List* V)
{
        if (!V) {
                perror ("variable list uninitialized");
        } else {
                size_t i = 0;
                while (i < V->num_var) {
                        printf ("Variable No: %zu ", i);
                        print_var (V->variables[i]);
                        i++;
                }
        }
}



void print_cond (Cond_Expr* C)
{
        if (C->kind == IF) {
                printf ("IF case\n");
        } else if (C->kind == ELSEIF) {
                printf ("ELSEIF case\n");
        } else if (C->kind == ELSE) {
                printf ("ELSE case\n");
        } else {
                printf ("Unknown Conditional Case\n");
        }

        printf ("Condition: \n");
        print_ast (C->cond);
        printf ("Case Body: \n");
        print_body (C->body);
        
        if (C->chain != NULL) {
                printf ("Conditional Chain: \n");
                print_cond (C->chain);
        }
}

void print_ast (Astn* A)
{
        if (!A) {
                printf ("This ast is empty...\n");
        } else if (A->kind == NODE_LITERAL) {
                printf ("Literal Node: \n");
                if (A->data.literal->type == LIT_INT) {
                        printf ("%d\n", A->data.literal->value.int_val);
                } else if (A->data.literal->type == LIT_CHAR) {
                        printf ("%c\n", A->data.literal->value.char_val);
                } else if (A->data.literal->type == LIT_VAR) {
                        Var* V = A->data.literal->value.var;
                        printf ("Variable: %s\n", V->name);
                        printf ("type: ");
                        print_type (V->type);
                        printf ("values: \n");
                        print_ast (V->value);
                } else {
                        printf ("Node literal but didnt match a type...\n");
                }
        } else if (A->kind == NODE_BINARY_EXPR) {
                printf ("Binary Expression Node: \n");
                printf ("Operation: %s\n", tokenType2string(A->data.binary->op));
                printf ("left: \n");
                print_ast (A->data.binary->left);
                printf ("right: \n");
                print_ast (A->data.binary->right);
        } else if (A->kind == NODE_UNARY_EXPR) {
                printf ("Unary Expression Node: \n");
                printf ("Operation: %s\n", tokenType2string(A->data.unary->op));
                printf ("Argument: \n");
                print_ast (A->data.unary->arg);
        } else if (A->kind == NODE_FUN_CALL) {
                printf ("Function Call Node: \n");
                printf ("Name: %s\n", A->data.fun_call->fun_name);
                printf ("Args: \n");
                size_t i = 0;
                while (i < A->data.fun_call->num_arg) {
                        print_ast (A->data.fun_call->args[i++]);
                }
        } else if (A->kind == NODE_LOOP) {
                printf ("Loop Node: \n");
                printf ("Condition: \n");
                print_ast (A->data.loop->cond);
                printf ("Body: \n");
                print_body (A->data.loop->body);
        } else if (A->kind == NODE_COND) {
                printf ("Conditional Node: \n");
                print_cond (A->data.cond);
        } else if (A->kind == NODE_BODY) {
                printf ("Body Node: \n");
                print_body (A->data.body_block);
        } else if (A->kind == NODE_LAMBDA) {
                printf ("Lambda Node :\n");
                printf ("Variable:\n");
                print_var (A->data.lambda->var);
                printf ("Function:\n");
                print_ast (A->data.lambda->function);
        } else if (A->kind == NODE_LAMCALL) {
                printf ("Lambda Call Node: \n");
                printf ("Argument:\n");
                print_ast (A->data.lam_call->arg);
                printf ("Function:\n");
                print_ast (A->data.lam_call->function);
        } else {
                printf ("AST Kind did not match?????\n");
        }
}

void print_body (Body_Block* body)
{
        printf ("Printing Body: \n");
        if (!body) {
                printf ("No Body... nobody but you!\n");
                return;
        }
        
        if (body->vars != NULL) {
                printf ("Variables in body scope: \n");
                print_varlist (body->vars);
        }

        printf ("Body Instructions: \n");
        size_t i = 0;
        while (i < body->num_inst) {
                print_ast (body->inst[i++]);
                
        }
        
}

void print_function (Astn* fun, size_t i)
{
        if (!fun) {
                perror ("printing fun, but fun isnt there.\n");
                exit (EXIT_FAILURE);
        }
        if (fun->kind != NODE_FUN_DEC) {
                printf ("function %zu didnt declare right", i);
        }       

        printf ("Function #%zu: '%s'\n", i, fun->data.fun_dec->fun_name);
        printf ("Return type: \n");
        print_type (fun->data.fun_dec->ret_type);

        printf ("Parameters: ");
        print_param (fun->data.fun_dec->variables[0]);

        printf ("Body: ");
        print_body (fun->data.fun_dec->body);

        printf ("Does it look any good?\n");
}

void print_user_type (User_Type* U)
{
        printf ("Name: %s, ", U->name);
        
        if (U->kind == STRUCT) {
                printf ("Kind: STRUCT...Printing Varlist:\n");
                print_varlist (U->data.Struct);
        } else {
                printf ("Kind: ALIAS...Printing Type\n");
                print_type (U->data.Alias);
        }
        
}

void print_GUser (GUser_Types* G)
{
        size_t i = 0;
        size_t n = G->num;

        while (i < n) {
                printf ("User type #%zu:\n", i);
                print_user_type (G->typelist[i++]);
        }
}

void print_ASTProgram (AST_Program* A) 
{
        printf ("Print debug of the nD parser\n");
        printf ("User Defined Types:\n");
        print_GUser (A->G);

        printf ("Functions:\n");
        
        size_t i = 0;
        while (i < A->function_count) {
                print_function (A->functions[i], i);
                
                i++;
        }

        printf ("Print_ASTProgam Complete.\n");
}



// ========================================================================= //
