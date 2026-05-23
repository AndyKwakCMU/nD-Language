#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
	TOK_EOF,

	TOK_IDENTIFIER,
	TOK_INT_LITERAL,

	TOK_FN,
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

	TOK_PLUS,
	TOK_MINUS,
	TOK_STAR,
	TOK_SLASH,

        TOK_ASSIGN,

	TOK_ERROR

} TokenType;

typedef struct {
	TokenType type;
	char lexeme[256];
} Token;

#endif
