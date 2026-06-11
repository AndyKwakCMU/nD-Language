// ========================================================================= //
// Andy Kwak 2026

// type implementation
// I like everything except the user defined types, this is a pain

// ========================================================================= //
#ifndef TYPE_H
#define TYPE_H

// ========================================================================= //


// ========================================================================= //
typedef enum {
        VALUE,
        POINTER,
        FUNCTION
} Type_Kind;

typedef Type;

typedef struct Type_Tree;

typedef struct Type_Tree {
        Type* input;
        Type* output;
} Type_Tree;

typedef enum {
        INT,
        CHAR,
        INT_MUT,
        CHAR_MUT
} Base_Type;

typedef struct {
        Type_Kind kind;
        union {
                Type_Tree* tree;

                Base_Type base;

                Type* pointer;

                // TODO: Later on, user defined types (HARD)
                // User_Type user;
        } data;
} Type;

// ========================================================================= //

#endif
