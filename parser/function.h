// ========================================================================= //
// Andy Kwak 2026

// My function implementation, I really like how neat this is
// ========================================================================= //
#ifndef FUNCTION_H
#define FUNCTION_H

#include "type.h"
#include "variable.h"

// ========================================================================= //
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

// ========================================================================= //

#endif 
