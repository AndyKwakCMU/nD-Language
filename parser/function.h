
#include "type.h"
#include "variable.h"


typedef struct Fun_Type {
        char* fun_name;
        Type* ret_type;
        int num_param;
        Var** params; // UBA
        Astn** body;
} Fun_Type;