// ========================================================================= //
// Andy Kwak 2026

// My variable implementation
// The closure implementation allows me to use functions as values, 
// the env is the array of variables the function has access to, 
// an example would be CPS where we have like a thousand variables

// ========================================================================= //
#ifndef VARIABLE_H
#define VARIABLE_H


#include "type.h"

// ========================================================================= //
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

#endif
