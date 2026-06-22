// ========================================================================= //
// Andy Kwak 2026

// type implementation
// I like everything except the user defined types, this is a pain

// ========================================================================= //
#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

// ========================================================================= //


// ========================================================================= //
typedef enum {
        VALUE,
        POINTER,
        FUNCTION,
        USER
} Type_Kind;

typedef Type;

typedef enum {
        INT,
        CHAR,
        INT_MUT,
        CHAR_MUT
} Base_Type;

typedef struct Type_Tree;

typedef struct Type_Tree {
        Type* input;
        Type* output;
} Type_Tree;

typedef struct User {
        char* cont_name;
        Type* cont_type;
} User;

typedef struct User_Type {
        char* struct_name;
        User** struct_types;
} User_Type;

typedef struct {
        Type_Kind kind;
        union {
                Type_Tree* tree;

                Base_Type base;

                Type* pointer;

                User_Type* user;
        } data;
} Type;

typedef struct Glo_User_Types {
        size_t index;
        size_t size;
        User_Type** typelist;
} GUser_Types;

// ========================================================================= //


// ========================================================================= //
// User types implementation
GUser_Types* new_GUser ();
void GUser_add (GUser_Types* G, User_Type* U);
bool in_GUser (GUser_Types* G, char* name);
User_Type* get_GUser (GUser_Types* G, char* name);


// ========================================================================= //


#endif
