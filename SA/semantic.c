// ========================================================================= //
// Andy Kwak 2026

// Semantic Analysis Implementation of the nD-Language
// This is where we rigorously type check, assign function arena size,
// define allocated memory scope, and complete some of the unfinished
// details of the AST.

/*

 Further details of the semantic analysis is that my AST with user structs
 mostly hold a placeholder, they need to be rigourous type checked as well
 as check for the existence of them. Alias types and such should be efficiently
 checked as well.

 Another set of concerns is checking if the type is mutable, and invalidating
 any mutations on non-mutable variables. This also applies to user struct
 variables. 

 We also have partial evaluation of functions, that will be type checked, 
 and make sure the output of the function, or the partial function, is 
 still valid logically and semantically in the program. 






*/
// ========================================================================= //


// ========================================================================= //
#include <stdlib.h>
#include <assert.h>

#include "../tokenizer/token.h"
#include "../parser/ast.h"
#include "../utils.h"

// ========================================================================= //


// ========================================================================= //
void semantic_analysis (AST_Program* A);



// ========================================================================= //


// ========================================================================= //







void semantic_analysis (AST_Program* A)
{

}







// ========================================================================= //

