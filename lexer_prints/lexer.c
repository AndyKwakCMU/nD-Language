// Andy Kwak 2026
// The Lexer reads every character to identify key words and values that are needed. Converting from human readable language to a token language that the computer can then optimize into an IR and then machine code. 

// Currently we will use a brute force scanning identifiers to find words.


#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"


void alpha_handler (int c, FILE* fptr);
void digit_handler (int c, FILE* fptr);
void swich_handler (int c);
void word_handler (char* word);


int lexer (FILE* fptr)
{
	int c;
	while ((c = fgetc (fptr)) != EOF) {
		if (c == '#') {
			while (c != '\n' && c != EOF) {
				c = fgetc (fptr);
			}
			continue;
		}
		if (isspace (c)) {
			continue;
		}
		
		if (isalpha (c) || c == '_') {
			alpha_handler (c, fptr);
			continue;
		}
		
		if (isdigit (c)) {
			digit_handler (c, fptr);
			continue;
		}

		swich_handler (c);
	}
	printf ("EOF\n");
	return 0;
}

void alpha_handler (int c, FILE* fptr)
{
	char word[256];
	int i = 0;

	word[i++] = c;

	while ((c = fgetc (fptr)) != EOF && 
	       (isalnum (c) || c == '_')) {
		word [i++] = c;
	}

	word[i] = '\0';
	
	if (c != EOF) {
		ungetc (c, fptr);
	}
	
	word_handler (word);
}

void word_handler (char* word) 
{
	if (strcmp (word, "fun") == 0) {
		printf ("FUN\n");
	} else if (strcmp (word, "let") == 0) {
		printf ("LET\n");
	} else if (strcmp (word, "return") == 0) {
		printf ("RETURN\n");
	} else if (strcmp (word, "int") == 0) {
		printf ("TYPE_INT\n");
	} else if (strcmp (word, "int$") == 0) {
		printf ("TYPE_INT_MUT\n");
	} else {
		printf ("IDENTIFIER (%s)\n", word);
	}
}

void digit_handler (int c, FILE* fptr)
{
	char number[256];
	int i = 0;

	number[i++] = c;

	while ((c = fgetc (fptr)) != EOF && isdigit (c)) {
		number[i++] = c;
	}
	
	number[i] = '\0';

	if (c != EOF) {
		ungetc (c, fptr);
	}

	printf ("INT_LITERAL(%s)\n", number);
}

void swich_handler (int c)
{
	switch (c) {
		case '(' :
			printf ("LPAREN\n");
			break;
		case ')' :
			printf ("RPAREN\n");
			break;
		case '{' :
			printf ("LBRACE\n");
			break;
		case '}' :
			printf ("RBRACE\n");
			break;
		case ':' :
			printf ("COLON\n");
			break;
		case ';' :
			printf ("SEMICOLON\n");
			break;
		case '+' :
			printf ("PLUS\n");
			break;
		case '-' :
			printf ("MINUS\n");
			break;
		case '*' :
			printf ("STAR\n");
			break;
		case '/' :
			printf ("SLASH\n");
			break;
		case '=' :
			printf ("ASSIGN\n");
			break;
		default:
			printf ("UNKNOWN (%c)\n", c);
			break;
	}
}
