
#include "type.h"

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
        Type* type;
        Value* value;
} Var;