// Andy Kwak 2026
// My AST struct implementation


typedef enum {
        // Literals like int values
        NODE_LITERAL,

        // Bracket shit fuck you
        NODE_SCOPE,

        //Takes two values and does something, like arithmetic expressions
        NODE_BINARY_EXPR,

        // Takes one value and does something, like a pointer dereference
        NODE_UNARY_EXPR,

        // Function
        NODE_FUN
} Node_Kind;

typedef struct AST_Node;

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

// Function delcaration stuff
typedef enum {
        TYPE_INT,
        TYPE_POINTER,
        TYPE_FUN,
        TYPE_NONE
} Fun_Ret_Type;

typedef struct Fun_Type;

typedef struct {
        Fun_Type* return_type;
        int param_count;
        Fun_Type** param_type;
} Fun_Sig_Data;

typedef struct Fun_Type {
        char* name;
        Fun_Ret_Type type;
        Fun_Type** params;
        union {
                // Pointer return type
                struct Fun_Type* pointer;

                // Function return type
                Fun_Sig_Data Fun_Sig;
        } data;
} Fun_Type;

typedef struct AST_Node {
        Node_Kind kind;
        union {
                Literal_Expr* literal;
                Binary_Expr* binary;
                Urnary_Expr* urnary;
                Fun_Type* fun;
        } data;
} Astn;

typedef struct {
        Astn** functions;
        int function_count;
        int capacity;
} AST_Program;

Astn* new_literal_astn (Literal_Expr* literal);
Astn* new_binary_astn  (Binary_Expr* binary);
Astn* new_urnary_expr  (Urnary_Expr* urnary);
Astn* new_fun          (Fun_Type* fun);
