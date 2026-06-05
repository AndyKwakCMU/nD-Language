#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
	TOK_EOF,

	TOK_IDENTIFIER,
	TOK_INT_LITERAL,

	TOK_FN,
	TOK_LAMBDA,
	TOK_FN_TYPE,
	TOK_RETURN,
	TOK_LET,
	TOK_INT,
	TOK_INT_MUT,

	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,

	TOK_COLON,
	TOK_SEMICOLON,

	TOK_IF,
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

typedef struct {
	TokenType type;
	char* lexeme;
} Token;

#endif
