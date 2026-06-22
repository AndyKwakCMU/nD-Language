// ========================================================================= //
// Andy Kwak 2026

// My AST struct implementation
// ========================================================================= //
#ifndef AST_H
#define AST_H

#include "type.h"
#include "variable.h"
#include "function.h"

// ========================================================================= //


// ========================================================================= //
Astn* new_literal_astn (Literal_Expr* literal);
Astn* new_binary_astn  (Binary_Expr* binary);
Astn* new_urnary_expr  (Urnary_Expr* urnary);
Astn* new_loop_expr    (Loop_Expr* loop);
Astn* new_fun_dec      (Fun_Type* fun_dec);
Astn* new_fun_call     (Fun_Call* fun_call);


AST_Program* new_Program ();
void program_add_fun (AST_Program* A, Fun_Type* fun);
void program_get_fun (AST_Program* A, char* name);

// ========================================================================= //


// ========================================================================= //
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

typedef struct AST_Node;

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
        // Also, scopes are values, you can return values or allocated memory from scopes,
        // Simplifying and organizing a lot of operations.
        Var**  var; 
        size_t num_var;
        size_t var_cap;
} Body_Block;

typedef struct {
        Binary_Expr* cond;
        Body_Block* body;
} Loop_Expr;

typedef struct AST_Node {
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
} Astn;

typedef struct {
        Astn** functions;
        size_t function_count;
        size_t capacity;
} AST_Program;

// ========================================================================= //


#endif
