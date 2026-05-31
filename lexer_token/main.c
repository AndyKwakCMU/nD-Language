// ========================================================================= //
// Andy Kwak 2026

// main file of the ndc compiler 
// ========================================================================= //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "token.h"
#include "tokenize.h"
#include "token_utils.h"


// ========================================================================= //
int main (int argc, char**argv)
{
	char* filename = NULL;
	int debug_mode = 0;

	filename = argv[1];
	
	if (argc >= 3 && strcmp (argv[2], "-d") == 0) {
		debug_mode = 1;
	}

	FILE* fcount = fopen (filename, "r");
	printf ("Word counter UTILS function, running...");
	int num_words = word_counter (fcount);
	printf ("EOF, COMPLETE.\n");
	printf ("Total words: %d\n", num_words);
	fclose (fcount);

	FILE* fptr = fopen (filename, "r");
	
	if (fptr == NULL) {
		printf ("\nFUCK YOU\n");
		return 1;
	} 
	
	Token** T = tokenize (fptr, num_words);
	print_tokens (T, num_words);
	
	
	fclose (fptr);
	free_tokens (T, num_words);

	printf ("Oh hell yeah!\n");
	return 0;
}
// ========================================================================= //
