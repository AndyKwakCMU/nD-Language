// ========================================================================= //
// Andy Kwak 2026

// The utilities file for tokens! Later on, everything should be moved into a 
// standalone library and make an interface where I wouldn't need to hardcode
// a thousand freaking switch cases every single time.
// ========================================================================= //


// ========================================================================= //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

// ========================================================================= //


// ========================================================================= //
void print_tokens (Token** T, int n);
char* tokenType2string (TokenType type);
void free_tokens (Token** T, int n);

// ========================================================================= //


// ========================================================================= //
// print_tokens - Helper function for printing out tokens into the terminal
void print_tokens (Token** T, int n)
//@requires isToken(T) && n >= 0;
//@ensures true;
{
        printf ("Printing Tokens...\n");
        int i = 0;
        
        while (i < n) {
                Token* t = T [i];
                printf ("Token #%d: %s  %s\n", i++, 
                        tokenType2string (t->type), 
                        t->lexeme);
        }
        printf ("Done!\n");
}

// tokenType2string - Turns token type enum objects into string literals
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
                case TOK_FN_TYPE :
                        return "TOK_FN_TYPE";
                case TOK_RETURN :
                        return "TOK_RETURN";
                case TOK_INT_TYPE :
                        return "TOK_INT";
                case TOK_INT_MUT_TYPE :
                        return "TOK_INT_MUT";
                case TOK_LPAREN : 
                        return "TOK_LPAREN";
                case TOK_RPAREN :
                        return "TOK_RPAREN";
                case TOK_LBRACE : 
                        return "TOK_LBRACE";
                case TOK_RBRACE : 
                        return "TOK_RBRANCE";
                case TOK_COLON:
                        return "TOK_COLON";
                case TOK_SEMICOLON :
                        return "TOK_SEMICOLON";
                case TOK_IF :
                        return "TOK_IF";
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
// ========================================================================= //


// ========================================================================= //
// free_tokens - Helper function for freeing a list of Tokens
void free_tokens (Token** T, int n)
//@requires isToken(T) && n >= 0 && \length(T) == n;
//@ensures true;
{
        int i = 0;

        while (i < n) {
                if (T[i]->lexeme != NULL) {
                        free(T[i]->lexeme);
                }
                free(T[i++]);
        }
        free(T);
}

// ========================================================================= //

