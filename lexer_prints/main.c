// main file of the ndc compiler 

#include <stdio.h>
#include <string.h>

#include "lexer.h"



int main (int argc, char**argv)
{
	char* filename = NULL;
	int debug_mode = 0;

	filename = argv[1];
	
	if (argc >= 3 && strcmp (argv[2], "-d") == 0) {
		debug_mode = 1;
	}

	FILE* fptr;
	fptr = fopen (filename, "r");
	
	if (fptr == NULL) {
		printf ("\nfuck you\n");
	} else {
		lexer (fptr);
	}
	fclose (fptr);
	return 0;
}

