// ========================================================================= //
// Andy Kwak 2026

// Token system for the tokenizer

// ========================================================================= //


// ========================================================================= //
#ifndef TOKEN_H
#define TOKEN_H


#include <stdlib.h>


// ========================================================================= //


// ========================================================================= //
typedef enum {
	TOK_EOF,

	TOK_IDENTIFIER,
	TOK_INT_LITERAL,
	TOK_CHAR_LITERAL,

	TOK_FN,
	TOK_LAMBDA,
	TOK_ARROW_TYPE,
	TOK_RETURN,
	TOK_TYPEDEF,

	TOK_MUT,
	TOK_INT_TYPE,
	TOK_CHAR_TYPE,
	TOK_NONE_TYPE,
	TOK_LIST_TYPE,
	TOK_STRING_TYPE,

	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_LSBRACE,
	TOK_RSBRACE,

	TOK_COLON,
	TOK_SEMICOLON,
	TOK_COMMA,
	TOK_DOT,         // Binary_Expr

	TOK_IF,
        TOK_ELSEIF,
	TOK_ELSE,        // Keyword

        
	TOK_EQ,          // Binary_Expr
        TOK_LT,          // Binary_Expr
	TOK_LEQ,         // Binary_Expr
	TOK_GT,          // Binary_Expr
	TOK_GEQ,         // Binary_Expr

	TOK_AND,         // Binary_Expr
	TOK_OR,          // Binary_Expr

	TOK_WHILE,       // Keyword

	TOK_PLUS,        // Binary_Expr
	TOK_MINUS,       // Binary_Expr
	TOK_STAR,        // Binary_Expr
	TOK_SLASH,       // Binary_Expr

	TOK_ADD_ASSIGN,  // Binary_Expr
	TOK_SUB_ASSIGN,  // Binary_Expr
	TOK_MUL_ASSIGN,  // Binary_Expr
	TOK_DIV_ASSIGN,  // Binary_Expr

        TOK_ASSIGN,      // Binary_Expr
	TOK_MATCH_ARROW, 
        TOK_NEW,

	TOK_ERROR,


	// Only for the array implementation of tokens
	TOK_START,
	TOK_END
} TokenType;

// ========================================================================= //


// ========================================================================= //
typedef struct Token {
	TokenType type;
	char* lexeme;
	size_t row;
	size_t col;
} Token;

// ========================================================================= //


#endif
