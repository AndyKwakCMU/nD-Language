// ========================================================================= //
// Andy Kwak 2026

// main file of the ndc compiler 
// ========================================================================= //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "utils.h"

#include "tokenizer/token.h"
#include "tokenizer/tokenize.h"
#include "tokenizer/real_stream.h"

#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/ast_util.h"

//#include "SA/semantic.h"

// printing the array of token shit
/*
#include "tokenizer/tokenize_array.h"
#include "tokenizer/token_utils.h"
*/





// ========================================================================= //
int main (int argc, char**argv)
{
	printf ("Running the ndc compiler...\n");

	if (argc < 2) {
                fprintf (stderr, "Error: No input file specified.\n");
                fprintf (stderr, "Usage: %s <filename> [-d]\n", argv[0]);
                return 1;
        }

	char* filename = NULL;
	int debug_mode = 0;

	filename = argv[1];

	printf ("Filename: %s\n", filename);

	if (argc >= 3 && strcmp (argv[2], "-d") == 0) {
		debug_mode = 1;
	}

	printf ("Debug Mode: %d\n", debug_mode);

	/*
	#ifdef DEBUG
	printf ("Printing stream ...\n");
	FILE* pstrm = fopen (filename, "r");
	Stream* P = new_stream (pstrm);
	print_stream (P);
	stream_free (P);
	fclose (pstrm);
	printf ("Sucess!\n");
	#endif
	*/

	printf ("Opening file ...");
	FILE* fptr = fopen (filename, "r");
	
	if (!fptr) {
		printf ("\nFUCK YOU\n");
		return 1;
	} 
	printf ("Sucess!\n");

	// token stream
	printf ("Building Token Stream ...");
	Stream* S = new_stream (fptr);
	printf ("Sucess!\n");

	printf ("Parsing ...");
	AST_Program* A = parse (S);
	printf ("Sucess!\n");

	stream_free (S);
	fclose (fptr);

	#ifdef DEBUG
	printf ("Printing Result ...");
	print_ASTProgram (A);
	printf ("Sucess!\n");
	#endif

	/*
	printf ("Analyzing Semantics of AST ...");
	semantic_analysis (A);
	#ifdef DEBUG
	print_ASTProgram (A);
	#endif
	printf ("Sucess!\n");
	*/


	/*
	printf ("Generating Bytecode ...");
	BC* B = IR_Comp (A);
	// free_ASTProgram (A);
	#ifdef DEBUG
	print_BC (B);
	#endif
	printf ("Sucess!\n");
	*/

	printf ("Oh hell yeah!\n");
	return 0;
}
// ========================================================================= //
