// ========================================================================= //
// Andy Kwak 2026

// Tokenize, a modified version of the lexer with a function next_token 
// being a lazily evaluated stream of tokens of the file. 

// Right now, we are going to run the entire stream and store all tokens in 
// order in an array. Just for debugging sake. Plus we don't really have a 
// parser implementation yet, so we can work the parser and stream of tokens 
// working very closely together later on. 
// ========================================================================= //


// ========================================================================= //

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "token.h"
#include "tokenize.h"

#include "real_stream.h"

#define MAX_WORD_LEN (sizeof(char) * 20)


// ========================================================================= //
// Building an array of tokens from the file

Token** tokenize (FILE* fptr, int num_words);
Token* next_token (FILE* fptr);
Token* peek_token (FILE* fptr);

Token* comment_handler (int c, FILE* fptr);
Token* alpha_handler (int c, FILE* fptr);
Token* word_handler (char* word);
Token* digit_handler (int c, FILE* fptr);
Token* switch_handler (int c, FILE* fptr);

// ========================================================================= //
// Using the array of tokens to make a fake stream of tokens
// debugging sake
// I will implement a real stream when I feel like it, is there a performance benefit?
// Technically yes no depending on how I use the tokens, algorithm sake I want a stream

Stream* new_stream (FILE* T);

Token* stream_curr (Stream* S);
Token* stream_next (Stream* S);
Token* stream_peek (Stream* S);

bool is_stream_end (Stream* S);


// ========================================================================= //
// tokenize - Creates an array of Tokens, populated with tokens in order
// of the source code. It allocates exactly the amount of tokens.
Token** tokenize (FILE* fptr, int num_words)
//@requires isFilePointer(fptr) && num_words >= 0;
//@ensures length(\result) == num_words;
{
	Token** T = malloc ((sizeof (Token) * num_words) + 2);
	int count = 0;
	T [count++] = TOK_START;

	while (count < num_words) {
		Token* t = next_token (fptr);
		if (t == NULL) {
			printf ("ERROR: num_words does not match actual number of words\n");
			break;
		}
		T [count++] = t;
	}
	T[count] = TOK_END;

	return T;
}
// ========================================================================= //


// ========================================================================= //
// next_token - Thunk of the file pointer giving out a stream of tokens until
// EOF. 
Token* next_token (FILE* fptr) 
//@requires isFilePointer(fptr);
//@ensures isToken(\result);
{
	int c = fgetc (fptr);
	if (c == EOF) {
		printf ("EOF\n");
		return NULL;
	}
	Token* t;
	//printf ("debug char (next_token): %c\n", (char) c);
	
	if (c == '#') {
	// Skips comments
		t = comment_handler (c, fptr);
	} else if (isspace (c)){
	// Skips spaces
		t = next_token (fptr);
	}else if (isalpha (c) || c == '_') {
	// Finds alphabet or underscore, hands off to helper handler
		t = alpha_handler (c, fptr);
	} else if (isdigit (c)) {
	// Finds digits, hands off to digit handler
		t = digit_handler (c, fptr);
	} else {
	// Last case, hand off to the character switch case
		t = switch_handler (c, fptr);
	}

	return t;
}

// comment_handler - Skips commented section of the source code
Token* comment_handler (int c, FILE* fptr)
{
	while (c != '\n' && c != EOF) {
		c = fgetc (fptr);
	}
	return next_token (fptr);
}

// alpha_handler - Detects the alphabet (ouu shii) and it tries to get the
// whole word, including underscores, and ending when it sees whitespace.
Token* alpha_handler (int c, FILE* fptr)
//@requires isalpha(c); 
{
	char* word = malloc (MAX_WORD_LEN);
	int i = 0;
	
	while (c != EOF &&
	       (isalnum (c) || c == '_')) {
		word [i++] = c;
		c = fgetc (fptr);
	}
	word [i] = '\0';
	//printf ("debug char (alpha_handler): %c\n", (char) c);

	if (c != EOF) {
		ungetc (c, fptr);
	}
	
	return word_handler (word);
}

// word_handler - Actually matches and allocates the Token struct
Token* word_handler (char* word)
{
	Token* t = malloc (sizeof (Token));
	t->lexeme = NULL;
	if (strcmp (word, "fun") == 0) {
		t->type = TOK_FN;
	} else if (strcmp (word, "fn") == 0){
		t->type = TOK_LAMBDA;
	} else if (strcmp (word, "if") == 0) {
		t->type = TOK_IF;
	} else if (strcmp (word, "elseif") == 0) {
                t->type = TOK_ELSEIF;
        } else if (strcmp (word, "else") == 0) {
		t->type = TOK_ELSE;
	} else if (strcmp (word, "while") == 0) {
		t->type = TOK_WHILE;
	} else if (strcmp (word, "let") == 0) {
		t->type = TOK_LET;
	} else if (strcmp (word, "return") == 0) {
		t->type = TOK_RETURN;
	} else if (strcmp (word, "int") == 0) {
		t->type = TOK_INT_TYPE;
	} else if (strcmp (word, "int$") == 0) {
		t->type = TOK_INT_MUT_TYPE;
	} else if (strcmp (word, "none") == 0) {
		t->type = TOK_NONE_TYPE;	
	} else {
		t->type = TOK_IDENTIFIER;
		t->lexeme = word;
	}
	return t;
}

// digit_handler - Detects numbers, creates a word, or the whole string of 
// numbers without whitespace. Then, allocates and populates the Token
// struct.
Token* digit_handler (int c, FILE* fptr) 
//@requires isdigit (c);
{
	char* number = malloc (MAX_WORD_LEN);
	int i = 0;
	
	while (c != EOF && isdigit (c)) {
		number [i++] = c;
		c = fgetc (fptr);
	}
	number [i] = '\0';
	//printf ("debug char (digit_handler): %c\n", (char) c);

	if (c != EOF) {
		ungetc (c, fptr);
	}

	Token* t = malloc (sizeof (Token));
	t->type = TOK_INT_LITERAL;
	t->lexeme = number;
	
	return t;
}

// switch_handler - Detects a single character after failing previous
// if-statements. Matches and allocates the Token struct. 
// Populates the struct with the error token upon not being able to match
// the character. This is a SYNTAX ERROR.
Token* switch_handler (int c, FILE* fptr) 
{
	Token* t = malloc (sizeof (Token));
	t->lexeme = NULL;
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
			c = fgetc (fptr);
			if (c == '>') {
				t->type = TOK_FN_TYPE;
			} else {
				ungetc (c, fptr);
				t->type = TOK_MINUS;
			}
			break;
		case '*' :
			t->type = TOK_STAR;
			break;
		case '/' :
			t->type = TOK_SLASH;
			break;
		case '=' :
			c = fgetc (fptr);
			if (c == '>') {
				t->type = TOK_MATCH_ARROW;
			}else {
				ungetc (c, fptr);
				t->type = TOK_ASSIGN;
			}
			break;
		default:
			t->type = TOK_ERROR;
			break;
	}
	
	return t;
}
// ========================================================================= //

Token* peek_token (FILE* fptr)
{
	int c = fgetc (fptr);
	if (c == EOF) {
		printf ("EOF\n");
		return NULL;
	}
	Token* t;
	//printf ("debug char (next_token): %c\n", (char) c);
	
	if (c == '#') {
	// Skips comments
		t = comment_handler (c, fptr);
	} else if (isspace (c)){
	// Skips spaces
		t = next_token (fptr);
	}else if (isalpha (c) || c == '_') {
	// Finds alphabet or underscore, hands off to helper handler
		t = alpha_handler (c, fptr);
	} else if (isdigit (c)) {
	// Finds digits, hands off to digit handler
		t = digit_handler (c, fptr);
	} else {
	// Last case, hand off to the character switch case
		t = switch_handler (c, fptr);
	}
	ungetc (c, fptr);

	return t;
}


// ========================================================================= //

/*
Stream* new_strem (Token** T)
{
	Stream* S = malloc (sizeof (Stream));
	S->T = T;
	S->index = 1;
	return S;
}

Token* stream_curr (Stream* S)
{
	return S->T [S->index];
}

Token* stream_next (Stream* S)
{
	S->index = S->index + 1;
	return S->T [S->index];
}

Token* stream_peek (Stream* S)
{
	S->index = S->index + 1;
	return S->T [(S->index)--];
}
*/

Stream* new_stream (FILE* fptr)
{
	Stream* S = malloc (sizeof (Stream));
	if (!S) perror ("yo wtf stream dawg\n");
	S->fptr = fptr;
	S->prev_token = TOK_START;
	S->curr_token = next_token (S->fptr);
	return S;
}

Token* stream_prev (Stream* S)
{
	return S->prev_token;
}

Token* stream_curr (Stream* S)
{
	return S->curr_token;
}

Token* stream_peek (Stream* S)
{
	return peek_token (S->fptr);
}

Token* stream_next (Stream* S)
{
	return next_token (S->fptr);
}

bool is_stream_end (Stream* S)
{
        if ((stream_curr(S))->type == TOK_EOF) {
                return true;
        } else {
                return false;
        }
}
