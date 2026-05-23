// Tokenize,  a modified version of the lexer with a function next_token being a lazily evaluated stream of tokens of the file. 
// Right now, we are going to run the entire stream and store all tokens in order in an array. Just for debugging sake. Plus we don't really have a parser implementation yet, so we can work the parser and stream of tokens working very closely together later on. 

#include <stdio.h>

#include "token.h"
#include "tokenize.h"

Token** tokenize (FILE* fptr, int num_words);
Token* next_token (FILE* fptr);

Token* comment_handler (int c, FILE* fptr);
Token* alpha_handler (int c, FILE* fptr);
Token* word_handler (char* word);
Token* digit_handler (int c, FILE* fptr);
Token* switch_handler (int c);

Token** tokenize (FILE* fptr, int num_words);
{
	Token** T = malloc (sizeof (Token) * num_words);
	int count = 0;
	while (count < num_words) {
		T [count] = next_token (fptr);
	}
	return T;
}

Token* next_token (FILE* fptr) 
{
	int c = fgetc (fptr);
	if (c == EOF) printf ("EOF\n");
	Token* t;

	if (c == '#') t = comment_handler (c, fptr);
	if (isspace (c)) t = next_token (fptr);
	if (isalpha (c) || c == '_') t = alpha_handler (c, fptr);
	if (isdigit (c)) t = digit_handler (c, fptr);
	t = switch_handler (c);
	
	return t;
}

Token* comment_handler (int c, FILE* fptr)
{
	while (c != '\n' && c != EOF) {
		c = fgetc (fptr);
	}
	return next_token (fptr);
}

Token* alpha_handler (int c, FILE* fptr)
{
	char word [256];
	int i = 0;
	
	word [i++] = c;
	
	while ((c = fgetc (fptr)) != EOF &&
	       (isalnum (c) || c == '_')) {
		word [i++] = c;
	}
	word [i] = '\0';
	
	if (c != EOF) {
		ungetc (c, fptr);
	}
	
	return word_handler (word);
}

Token* word_handler (char* word)
{
	Token* t = malloc (sizeof (Token));
	*(t->lexeme) = "";
	if (strcmp (word, "fun") == 0) {
		t->type = TOK_FN;
	} else if (strcmp (word, "let") == 0) {
		t->type = TOK_LET;
	} else if (strcmp (word, "return") == 0) {
		t->type = TOK_RETURN;
	} else if (strcmp (word, "int") == 0) {
		t->type = TOK_INT;
	} else if (strcmp (word, "int$") == 0) {
		t->type = TOK_INT_MUT;
	} else {
		t->type = TOK_IDENTIFIER;
		*(t->lexeme) = word;
	}
	return t;
}

Token* digit_handler (int c, FILE* fptr) 
{
	char number [256];
	int i = 0;
	
	number [i++] = c;
	
	while ((c = fgetc (fptr)) != EOF && isdigit (c)) {
		number [i++] = c;
	}
	number [i] = '\0';

	if (c != EOF) {
		ungetc (c, fptr);
	}

	Token* t = malloc (sizeof (Token));
	t->type = TOK_INT_LITERAL;
	*(t->lexeme) = number;
	
	return t;
}

Token* switch_handler (int c) 
{
	Token* t = malloc (sizeof (Token));
	*(t->lexeme) = "";
	switch (c) {
		case '(' :
			t->type = TOK_LPAREN;
			break;
		case ')' :
			t->type = TOK_RPAREN;
			break;
		case '{' :
			t->type = TOK_LBRACE;
			break;
		case '}' :
			t->type = TOK_RBRACE;
			break;
		case ':' :
			t->type = TOK_COLON;
			break;
		case ';' :
			t->type = TOK_SEMICOLON;
			break;
		case '+' :
			t->type = TOK_PLUS;
			break;
		case '-' :
			t->type = TOK_MINUS;
			break;
		case '*' :
			t->type = TOK_STAR;
			break;
		case '/' :
			t->type = TOK_SLASH;
			break;
		case '=' :
			t->type = TOK_ASSIGN;
			break;
		default:
			t->type = TOK_ERROR;
			break;
	}
	
	return t;
}
