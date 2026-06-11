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
        int num_param;
        Var** params; // UBA
        Astn** body;
} Fun_Type;

typedef struct Fun_Call {
        char* fun_name;
        int num_args;
        Var** args; // UBA
} Fun_Call;

// ========================================================================= //

#endif 
