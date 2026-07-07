
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "utils.h"
#include "tokenizer/token.h"
#include "tokenizer/real_stream.h"
#include "tokenizer/tokenize.h"

bool switch_counter (int c, FILE* fcount) 
{
	switch (c) {
		case '(' :
			return true;
		case ')' :
			return true;
		case '{' :
			return true;
		case '}' :
			return true;
		case ':' :
			return true;
		case ';' :
			return true;
		case '+' :
			return true;
		case '-' :
			c = fgetc (fcount);
			if (c == '>') {
				return true;
			} else {
				ungetc (c, fcount);
				return true;
			}
		case '*' :
			return true;
		case '/' :
			return true;
		case '=' :
			c = fgetc (fcount);
			if (c == '>') {
				return true;
			} else {
				ungetc (c, fcount);
				return true;
			}
		default:
			return false;
	}
}

int word_counter (FILE* fcount)
{
        int count = 0;
	int c = fgetc (fcount);
        
	while (c != EOF) {
		//printf ("curr c: %d; curr i: %d\n", c, count);
		if (c == '#') {
			while (c != '\n' && c != EOF) {
				c = fgetc (fcount);
			}
		} else if (isspace (c)) {
			c = fgetc (fcount);
			continue;
		} else if (isalpha (c) || c == '_') {
			while (c != EOF &&
                                (isalpha (c) || c == '_')) {
					c = fgetc (fcount);
				}
			count++;
		} else if (isdigit (c)) {
			while (c != EOF && 
                                isdigit (c)) {
					c = fgetc (fcount);
				} 
			count++;
                } else {
			if (switch_counter (c, fcount)) {
				count++;
				c = fgetc (fcount);
			} 
		}
	}
	return count;
}

char* tokenType2string (TokenType type)
{
        switch (type) {
                case TOK_EOF :
                        return "TOK_EOF";
                case TOK_IDENTIFIER :
                        return "TOK_IDENTIFIER";
                case TOK_INT_LITERAL :
                        return "TOK_INT_LITERAL";
                case TOK_FN :
                        return "TOK_FN";
                case TOK_LAMBDA :
                        return "TOK_LAMBDA";
                case TOK_ARROW_TYPE :
                        return "TOK_ARROW_TYPE";
                case TOK_RETURN :
                        return "TOK_RETURN";
                case TOK_TYPEDEF :
                        return "TOK_TYPEDEF";
                case TOK_INT_TYPE :
                        return "TOK_INT";
                case TOK_INT_MUT_TYPE :
                        return "TOK_INT_MUT";
                case TOK_CHAR_TYPE :
                        return "TOK_CHAR_TYPE";
                case TOK_CHAR_MUT_TYPE :
                        return "TOK_CHAR_MUT_TYPE";
                case TOK_LPAREN : 
                        return "TOK_LPAREN";
                case TOK_RPAREN :
                        return "TOK_RPAREN";
                case TOK_LBRACE : 
                        return "TOK_LBRACE";
                case TOK_RBRACE : 
                        return "TOK_RBRANCE";
                case TOK_LSBRACE :
                        return "TOK_LSBRACE";
                case TOK_RSBRACE :
                        return "TOK_RSBRACE";
                case TOK_COLON:
                        return "TOK_COLON";
                case TOK_COMMA :
                        return "TOK_COMMA";
                case TOK_SEMICOLON :
                        return "TOK_SEMICOLON";
		case TOK_DOT :
			return "TOK_DOT";
                case TOK_IF :
                        return "TOK_IF";
                case TOK_ELSEIF :
                        return "TOK_ELSEIF";
                case TOK_ELSE :
                        return "TOK_ELSE";
                case TOK_WHILE :
                        return "TOK_WHILE";
                case TOK_PLUS :
                        return "TOK_PLUS";
                case TOK_MINUS :
                        return "TOK_MINUS";
                case TOK_STAR :
                        return "TOK_STAR";
                case TOK_SLASH :
                        return "TOK_SLASH";
                case TOK_ASSIGN :
                        return "TOK_ASSIGN";
                case TOK_MATCH_ARROW :
                        return "TOK_MATCH_ARROW";
                case TOK_ERROR :
                        return "i'm in... 0.3 seconds!";
                default:
                        return "fuck you bitch!";
        }
}

void print_token (Token* tok)
{
        TokenType t = tok->type;
        printf ("Token: %s", tokenType2string (t));
        if (t == TOK_IDENTIFIER) {
                printf (", lexeme: '%s'", tok->lexeme);
        }
        printf ("\n");   
}

void print_stream (Stream* S)
{
	int i = 0;
	while (!is_stream_end (S)) {
		printf ("#%d - ", i++);
		print_token (stream_curr(S));
		stream_next (S);
	}
}