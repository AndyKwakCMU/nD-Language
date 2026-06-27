// ========================================================================= //
// Andy Kwak 2026

// Printing the AST Program

// ========================================================================= //

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

// ========================================================================= //


// ========================================================================= //
void print_type (Type* type)
{
        if (type->kind == VALUE) {
                switch (type->data.base) {
                        case INT :
                                printf ("int\n");
                                break;
                        case INT_MUT :
                                printf ("mutable int\n");
                                break;
                        case CHAR :
                                printf ("char\n");
                                break;
                        case CHAR_MUT :
                                printf ("mutable char\n");
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
        } else { // USER
                printf ("User struct: %s\n", type->data.user->struct_name);
                size_t i = 0;
                size_t n = type->data.user->num_struct;
                while (i <= n) {
                        printf ("cont_name: %s\n", 
                                type->data.user->struct_types[i]->cont_name);
                        print_type (type->data.user->struct_types[i]->cont_type);
                }
        }
}

void print_param (Var** params, size_t n)
{
        if (!params) {
                perror ("parameter list uninitialized");
                exit (EXIT_FAILURE);
        }

        size_t i = 0;
        while (i < n) {
                printf ("Param #%zu name: '%s', Type: \n", i, params[i]->name);
                print_type (params[i]->type);
                i++;
        }
}

void print_body (Body_Block* body);

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
        print_param (fun->data.fun_dec->params, 
                   fun->data.fun_dec->num_param);

        //printf ("Body: ");
        //print_body (fun->data.fun_dec->body);

        printf ("Does it look any good?\n");
}

void print_ASTProgram (AST_Program* A) 
{
        printf ("Print debug of the nD parser\n");
        printf ("Functions:\n");
        
        size_t i = 0;
        while (i < A->function_count) {
                print_function (A->functions[i], i);
                i++;
        }

        printf ("Print_ASTProgam Complete.\n");
}



// ========================================================================= //
