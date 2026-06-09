


typedef enum {
        VALUE,
        POINTER,
        FUNCTION
} Type_Kind;


typedef Type;
typedef struct Type_Tree;

typedef struct Type_Tree {
        Type type;
        struct Type_Tree* input;
        struct Type_Tree* output;
} Type_Tree;

typedef enum {
        INT,
        CHAR,
        INT_MUT,
        CHAR_MUT
} Base_Type;

typedef struct Pointer_Type {
        Type* pointing;
} Pointer_Type;

typedef struct {
        Type_Kind kind;
        union {
                Type_Tree tree;

                Base_Type base;

                Pointer_Type pointer;

                // TODO: Later on, user defined types (HARD)
                // User_Type user;
        } data;
} Type;