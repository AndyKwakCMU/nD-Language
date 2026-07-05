// ========================================================================= //
// Andy Kwak 2026

// Token system for the tokenizer

// ========================================================================= //


// ========================================================================= //
#ifndef TOKEN_H
#define TOKEN_H

// ========================================================================= //


// ========================================================================= //
typedef enum {
	TOK_EOF,

	TOK_IDENTIFIER,
	TOK_INT_LITERAL,
	TOK_BOOL_TRUE,
	TOK_BOOL_FALSE,

	TOK_FN,
	TOK_LAMBDA,
	TOK_ARROW_TYPE,
	TOK_RETURN,
	TOK_TYPEDEF,

	TOK_INT_TYPE,
	TOK_INT_MUT_TYPE,
	TOK_CHAR_TYPE,
	TOK_CHAR_MUT_TYPE,
	TOK_BOOL_TYPE,
	TOK_BOOL_MUT_TYPE,
	TOK_NONE_TYPE,

	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_LSBRACE,
	TOK_RSBRACE,

	TOK_COLON,
	TOK_SEMICOLON,
	TOK_COMMA,

	TOK_IF,
        TOK_ELSEIF,
	TOK_ELSE,

	TOK_WHILE,

	TOK_PLUS,
	TOK_MINUS,
	TOK_STAR,
	TOK_SLASH,

        TOK_ASSIGN,
	TOK_MATCH_ARROW,

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
} Token;

// ========================================================================= //


#endif
