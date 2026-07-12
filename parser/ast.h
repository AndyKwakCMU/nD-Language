// ========================================================================= //
// Andy Kwak 2026

// My AST struct implementation
// ========================================================================= //
#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../tokenizer/token.h"

// ========================================================================= //


// ========================================================================= //
// Type Implementation
typedef enum {
        VALUE,
        POINTER,
        FUNCTION,
        USER
} Type_Kind;

typedef struct Type Type;

typedef enum {
        INT,
        CHAR,
        INT_MUT,
        CHAR_MUT
} Base_Type;

typedef struct Type_Tree Type_Tree;

struct Type_Tree {
        Type* input;
        Type* output;
};

typedef struct User {
        char* cont_name;
        Type* cont_type;
} User;

typedef struct User_Type {
        char* struct_name;
        User** struct_types;
        size_t num_struct;
        size_t struct_cap;
} User_Type;

struct Type {
        Type_Kind kind;
        union {
                Type_Tree* tree;

                Base_Type base;

                Type* pointer;

                User_Type* user;
        } data;
};

typedef struct Glo_User_Types {
        size_t index;
        size_t size;
        User_Type** typelist;
} GUser_Types;

// ========================================================================= //


// ========================================================================= //
// Variable Implementation
typedef struct AST_Node Astn;

typedef struct User_Var User_Var;

typedef struct Var_List Var_List;

typedef struct Var Var;

typedef struct User_Val {
        char* cont_name;
        Astn* cont_val;
} User_Val;

struct User_Var {
        char* struct_name;
        User_Val** struct_types;
        size_t num_struct;
        size_t struct_cap;
};

struct Var {
        char* name;
        Type* type;
        Astn* value;
};

struct Var_List {
        Var** variables;
        size_t num_var;
        size_t var_cap;
};
// ========================================================================= //


// ========================================================================= //
// AST nodes/tree implementation
typedef enum {
        // Literals like int values
        NODE_LITERAL,

        // Takes two values and does something, like arithmetic expressions,
        // conditionals, assignment, etc.
        NODE_BINARY_EXPR,

        // Takes one value and does something, like a pointer dereference
        NODE_UNARY_EXPR,

        // Function call
        NODE_FUN_CALL,

        // Function declaration
        NODE_FUN_DEC,

        // Loop block
        NODE_LOOP,

        // Conditional Block
        NODE_COND, 

        // Recursively defined body block
        NODE_BODY
} Node_Kind;



typedef struct {
        enum {
                LIT_INT,
                LIT_CHAR,
                LIT_VAR
        } type;
        union {
                int int_val;
                char char_val;
                Var* var;
        } value;
} Literal_Expr;

typedef struct {
        TokenType op;
        Astn* left;
        Astn* right;
} Binary_Expr;

typedef struct {
        TokenType op;
        Astn* arg;
} Unary_Expr;

typedef struct {
        Astn** inst; // instructions, UBA of ast, L->R order
        size_t num_inst;
        size_t inst_cap;

        // Every body enclosed by braces are a newly defined scope,
        // in my language, each defined scope owns its delcared allocated memory
        // unless otherwise returned, which the caller function will own 
        // the memory. A very Rust approach to memory allocations, except
        // I restrict progammers in a way they are enclosed in a box of
        // scopes where the compiler really can't mess up where a pointer's scope ends.

        // Thus each body has the capabilities to declare its own variables
        Var_List* vars;
} Body_Block;

typedef struct {
        Astn* cond;
        Body_Block* body;
} Loop_Expr;

typedef struct Cond_Expr Cond_Expr;
struct Cond_Expr {
        enum {
                IF,
                ELSEIF,
                ELSE
        } kind;
        Astn* cond;
        Body_Block* body;
        // LL approach
        Cond_Expr* chain;
};

typedef struct Fun_Type {
        char* fun_name;
        Type* ret_type;

        Var_List** variables;
        size_t num_var;
        size_t var_cap;

        Body_Block* body;
} Fun_Type;

typedef struct Fun_Call {
        char* fun_name;

        Astn** args;
        size_t num_arg;
        size_t arg_cap;
} Fun_Call;

struct AST_Node {
        Node_Kind kind;
        union {
                Literal_Expr* literal;
                Binary_Expr*  binary;
                Unary_Expr*   unary;
                Loop_Expr*    loop;
                Cond_Expr*    cond;
                Fun_Type*     fun_dec;
                Fun_Call*     fun_call;
                Body_Block*   body_block;
        } data;
};

typedef struct {
        Astn** functions;
        size_t function_count;
        size_t capacity;
} AST_Program;

// ========================================================================= //



// Helper functions so I dont lose hair
// new_struct  ()
// in_struct   ()
// get_struct  ()
// struct_add  ()
// free_struct ()
// ========================================================================= //
// User types implementation
GUser_Types* new_GUser ();

void GUser_add (GUser_Types* G, User_Type* U);

bool isin_GUser (GUser_Types* G, char* name);

User_Type* get_GUser (GUser_Types* G, char* name);

// ========================================================================= //
Var_List* new_varlist ();

void varlist_add (Var_List* L, Var* var);

bool isin_varlist (Var_List* V, char* name);

Var* varlist_get_var (Var_List* V, char* name);

void varlist_free (Var_List* L);

// ========================================================================= //
Fun_Type* new_fun (char* name);

bool isin_fun_varlist (Fun_Type* F, char* name);

Var* fun_varlist_get_var (Fun_Type* F, char* name);

void fun_var_dec_add (Fun_Type* F, Var_List* L);

void fun_param_add (Fun_Type* F, Var* v);

// ========================================================================= //
void call_add_arg (Fun_Call* C, Astn* arg);

// ========================================================================= //
Body_Block* new_body ();

void body_add_inst (Body_Block* B, Astn* inst);

void body_add_var (Body_Block* B, Var* v);

Var_List* body_get_varlist (Body_Block* B);

// ========================================================================= //
AST_Program* new_Program ();

void program_add_fun (AST_Program* A, Fun_Type* fun);

bool isin_program_fun (AST_Program* A, char* name);

Fun_Type* program_get_fun (AST_Program* A, char* name);


// ========================================================================= //


#endif
