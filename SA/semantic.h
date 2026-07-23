// ========================================================================= //
// Andy Kwak 2026

// Semantic Analysis Header of the nD-Language
// ========================================================================= //


// ========================================================================= //
#include <stdlib.h>
#include <assert.h>

#include "../tokenizer/token.h"
#include "../parser/ast.h"
#include "../utils.h"

// ========================================================================= //


// ========================================================================= //
typedef struct Prog Prog;

typedef struct List List;

struct List {
        size_t num;
        size_t cap;
        size_t size;
        void* list; 
};

struct Prog {
        AST_Program* A;
        size_t tot_arena_size;
        
        size_t heap_num;
        size_t heap_cap;
        List** heap_map;
        
};




// ========================================================================= //


// ========================================================================= //
Prog* semantic_analysis (AST_Program* A);



// ========================================================================= //