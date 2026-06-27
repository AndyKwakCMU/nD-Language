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
typedef struct Value Value;

typedef struct Closure {
        void* function_ptr;
        Value** env; // Array of variables the function has access to
} Closure;

struct Value {
        union {
                int integer;

                char character;

                Value* pointer;

                Closure closure;
        } as;
};

typedef struct {
        char* name;
        Type* type;
        Value* value;
} Var;
// ========================================================================= //


// ========================================================================= //
// AST nodes/tree implementation
typedef enum {
        // Literals like int values
        NODE_LITERAL,

        // Bracket shit fuck you
        NODE_SCOPE,

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

        // Recursively defined body block
        NODE_BODY
} Node_Kind;

typedef struct AST_Node Astn;

typedef struct {
        Type* type;
        union {
                int int_val;
                char char_val;
        } value;
} Literal_Expr;

typedef struct {
        char op;
        Astn* left;
        Astn* right;
} Binary_Expr;

typedef struct {
        char op;
        Astn* arg;
} Urnary_Expr;

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
        Var**  var; 
        size_t num_var;
        size_t var_cap;
} Body_Block;

typedef struct {
        Binary_Expr* cond;
        Body_Block* body;
} Loop_Expr;

typedef struct Fun_Type {
        char* fun_name;
        Type* ret_type;

        Var** params; // UBA
        size_t num_param;
        size_t param_cap;

        Body_Block* body;
} Fun_Type;

typedef struct Fun_Call {
        char* fun_name;

        Var** args; // UBA
        size_t num_args;
        size_t args_cap;
} Fun_Call;

struct AST_Node {
        Node_Kind kind;
        union {
                Literal_Expr* literal;
                Binary_Expr*  binary;
                Urnary_Expr*  urnary;
                Loop_Expr*    loop;
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


// ========================================================================= //
// User types implementation
GUser_Types* new_GUser ();
void GUser_add (GUser_Types* G, User_Type* U);
bool in_GUser (GUser_Types* G, char* name);
User_Type* get_GUser (GUser_Types* G, char* name);

// ========================================================================= //
void fun_param_add (Fun_Type*, Var* v);


AST_Program* new_Program ();
void program_add_fun (AST_Program* A, Fun_Type* fun);
void program_get_fun (AST_Program* A, char* name);

// ========================================================================= //


#endif
