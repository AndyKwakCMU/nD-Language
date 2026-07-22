// ========================================================================= //
// Andy Kwak 2026

// Lazily evaluated tokenizer

// ========================================================================= //


// ========================================================================= //

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>

#include "token.h"
#include "tokenize.h"
#include "real_stream.h"

#include "../utils.h"

#define MAX_WORD_LEN (sizeof(char) * 20)


// ========================================================================= //
// Building an array of tokens from the file

Token** tokenize (FILE* fptr, int num_words);
Token* next_token (FILE* fptr, dbug* D);

Token* comment_handler (int c, FILE* fptr, dbug* D);
Token* alpha_handler (int c, FILE* fptr, dbug* D);
Token* word_handler (char* word, dbug* D);
Token* digit_handler (int c, FILE* fptr, dbug* D);
Token* switch_handler (int c, FILE* fptr, dbug* D);

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
	Token* start = malloc (sizeof (Token));
	start->type = TOK_START;
	T [count++] = start;
	dbug* D = malloc (sizeof (dbug));
	D->col = 1;
	D->row = 1;

	while (count < num_words) {
		Token* t = next_token (fptr, D);
		if (t == NULL) {
			printf ("ERROR: num_words does not match actual number of words\n");
			break;
		}
		T [count++] = t;
	}
	Token* end = malloc (sizeof (Token));
	end->type = TOK_END;
	T[count] = end;

	return T;
}
// ========================================================================= //


// ========================================================================= //
// next_token - Thunk of the file pointer giving out a stream of tokens until
// EOF. 
Token* next_token (FILE* fptr, dbug* D) 
//@requires isFilePointer(fptr);
//@ensures isToken(\result);
{
	int c = fgetc (fptr);
	Token* t;


	if (c == EOF) {
		t = malloc (sizeof (Token));
		t->type = TOK_EOF;
		t->lexeme = NULL;
		return t;
	} else if (c == '#') {
	// Skips comments
		t = comment_handler (c, fptr, D);
	} else if (isspace (c)){
	// Skips spaces
		if (c == '\n') {
			D->row++;
			D->col = 1;
		} else {
			D->col++;
		}
		t = next_token (fptr, D);
	}else if (isalpha (c) || c == '_') {
	// Finds alphabet or underscore, hands off to helper handler
		t = alpha_handler (c, fptr, D);
	} else if (isdigit (c)) {
	// Finds digits, hands off to digit handler
		t = digit_handler (c, fptr, D);
	} else {
	// Last case, hand off to the character switch case
		t = switch_handler (c, fptr, D);
	}

	return t;
}

// comment_handler - Skips commented section of the source code
Token* comment_handler (int c, FILE* fptr, dbug* D)
{
	while (c != '\n' && c != EOF) {
		c = fgetc (fptr);
	}
	D->col = 1;
	D->row += 1;
	return next_token (fptr, D);
}

// alpha_handler - Detects the alphabet (ouu shii) and it tries to get the
// whole word, including underscores, and ending when it sees whitespace.
Token* alpha_handler (int c, FILE* fptr, dbug* D)
//@requires isalpha(c); 
{
	char* word = malloc (sizeof (char) * MAX_WORD_LEN);
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

	Token* t = word_handler (word, D);
	D->col += i;

	return t;
}

// word_handler - Actually matches and allocates the Token struct
Token* word_handler (char* word, dbug* D)
{
	Token* t = malloc (sizeof (Token));
	t->lexeme = NULL;
	if (strcmp (word, "fun") == 0) {
		t->type = TOK_FN;
	} else if (strcmp (word, "fn") == 0){
		t->type = TOK_LAMBDA;
	} else if (strcmp (word, "typedef") == 0){
		t->type = TOK_TYPEDEF;
	} else if (strcmp (word, "if") == 0) {
		t->type = TOK_IF;
	} else if (strcmp (word, "elseif") == 0) {
                t->type = TOK_ELSEIF;
        } else if (strcmp (word, "and") == 0) {
		t->type = TOK_AND;
	} else if (strcmp (word, "or") == 0) {
		t->type = TOK_OR;
	} else if (strcmp (word, "else") == 0) {
		t->type = TOK_ELSE;
	} else if (strcmp (word, "while") == 0) {
		t->type = TOK_WHILE;
	} else if (strcmp (word, "return") == 0) {
		t->type = TOK_RETURN;
	} else if (strcmp (word, "int") == 0) {
		t->type = TOK_INT_TYPE;
	} else if (strcmp (word, "char") == 0) {
		t->type = TOK_CHAR_TYPE;
	} else if (strcmp (word, "list") == 0) {
		t->type = TOK_LIST_TYPE;
	} else if (strcmp (word, "string") == 0) {
		t->type = TOK_STRING_TYPE;
	} else if (strcmp (word, "none") == 0) {
		t->type = TOK_NONE_TYPE;	
	} else {
		t->type = TOK_IDENTIFIER;
		t->lexeme = word;
	}
	t->row = D->row;
	t->col = D->col;
	return t;
}

// digit_handler - Detects numbers, creates a word, or the whole string of 
// numbers without whitespace. Then, allocates and populates the Token
// struct.
Token* digit_handler (int c, FILE* fptr, dbug* D) 
//@requires isdigit (c);
{
	char* number = malloc (sizeof (char) * MAX_WORD_LEN);
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

	t->row = D->row;
	t->col = D->col;

	D->col += i;
	
	return t;
}

// switch_handler - Detects a single character after failing previous
// if-statements. Matches and allocates the Token struct. 
// Populates the struct with the error token upon not being able to match
// the character. This is a SYNTAX ERROR.
Token* switch_handler (int c, FILE* fptr, dbug* D) 
{
	Token* t = malloc (sizeof (Token));
	t->lexeme = NULL;
	t->row = D->row;
	t->col = D->col;
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
		case '[' :
			t->type = TOK_LSBRACE;
			break;
		case ']' :
			t->type = TOK_RSBRACE;
			break;
		case ':' :
			t->type = TOK_COLON;
			break;
		case ';' :
			t->type = TOK_SEMICOLON;
			break;
		case ',' :
			t->type = TOK_COMMA;
			break;
		case '+' :
			t->type = TOK_PLUS;
			break;
		case '-' :
			c = fgetc (fptr);
			if (c == '>') {
				t->type = TOK_ARROW_TYPE;
				D->col++;
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
				D->col++;
			} else if (c == '=') {
				t->type = TOK_EQ;
				D->col++;
			} else {
				ungetc (c, fptr);
				t->type = TOK_ASSIGN;
			}
			break;
		case '<' :
			c = fgetc (fptr);
			if (c == '=') {
				t->type = TOK_LEQ;
				D->col++;
			} else {
				ungetc (c, fptr);
				t->type = TOK_LT;
			}
			break;
		case '>' :
			c = fgetc (fptr);
			if (c == '=') {
				t->type = TOK_GEQ;
				D->col++;
			} else {
				ungetc (c, fptr);
				t->type = TOK_GT;
			}
			break;
		case '$' :
			t->type = TOK_MUT;
			break;
		default:
			t->type = TOK_ERROR;
			break;
	}
	D->col++;
	return t;
}
// ========================================================================= //


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

// Stream implementation, very neat and nice!
// ========================================================================= //
Stream* new_stream (FILE* fptr)
{
	Stream* S = malloc (sizeof (Stream));
	if (!S) perror ("yo wtf stream dawg\n");
	S->fptr = fptr;
	Token* start = malloc (sizeof (Token));
	start->type = TOK_START;
	S->prev_token = start;

	dbug* D = malloc (sizeof (dbug));
	D->col = 1; D->row = 1;
	S->D = D;

	S->curr_token = next_token (S->fptr, S->D);
	S->next_token = next_token (S->fptr, S->D);
	return S;
}

Token* stream_prev (Stream* S)
{
	return S->prev_token;
}

Token* stream_curr (Stream* S)
{
	REQUIRES (S != NULL && S->curr_token != NULL);
	return S->curr_token;
}

Token* stream_peek (Stream* S)
{
	REQUIRES (S != NULL && S->next_token != NULL);
	return S->next_token;
}

Token* stream_next (Stream* S)
{
	REQUIRES (S != NULL && S->curr_token != NULL);
	REQUIRES (S->prev_token != NULL && S->next_token != NULL);
	
	Token* prev = S->prev_token;
	if (prev->lexeme != NULL) {
		free (prev->lexeme);
	}
	free (prev);

	S->prev_token = S->curr_token;
	S->curr_token = S->next_token;
	S->next_token = next_token (S->fptr, S->D);
	
	ENSURES (S != NULL && S->curr_token != NULL);
	ENSURES (S->next_token != NULL);
	return S->curr_token;
}

bool is_stream_end (Stream* S)
{
        if ((stream_curr(S))->type == TOK_EOF) {
                return true;
        } else {
                return false;
        }
}

void stream_free (Stream* S) {
	Token* tok = stream_prev(S);
	if (tok->lexeme != NULL) {
		free (tok->lexeme);
	}
	free (tok);
	tok = stream_curr (S);
	
	while (tok->type != TOK_EOF) {
		tok = stream_next (S);
		if (tok->lexeme != NULL) {
			free (tok->lexeme);
		}
		free (tok);
		tok = stream_peek (S);
	}
	free (tok);
	free (S->next_token);
	free (S->D);
	free (S);
	// no need to free the file pointer
}
// ========================================================================= //

