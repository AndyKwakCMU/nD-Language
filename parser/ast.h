// Andy Kwak 2026
// My AST struct implementation


typedef enum {
        // Literals like int values
        NODE_LITERAL,

        // Bracket shit fuck you
        NODE_SCOPE,

        // Takes two values and does something, like arithmetic expressions
        NODE_BINARY_EXPR,

        // Takes one value and does something, like a pointer dereference
        NODE_UNARY_EXPR,

        // Function calling and doing something idk fuck you
        NODE_FUNC_CALL
} Node_Type;

typedef struct AST_Node Astn;

typedef struct {
        int value;
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

struct AST_Node {
        Node_Type type;
        union {
                Literal_Expr literal;
                Binary_Expr binary;
                Urnary_Expr urnary;
        } data;
};

typedef struct {
        Astn** functions;
        int function_count;
        int capacity;
} AST_Program;

Astn* new_literal_astn (Node_Type type, Literal_Expr literal);
Astn* new_binary_astn (Node_Type type, Binary_Expr binary);
Astn* new_urnary_expr (Node_Type type, Urnary_Expr urnary);