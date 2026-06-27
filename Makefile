CC = gcc
CFLAGS = -Wall -g -std=c99


all: main.c 
	$(CC) $(CFLAGS) -o ndc_ast_print \
	main.c \
	utils.c \
	tokenizer/tokenize.c \
	parser/parser.c \
	parser/ast_util.c \
	parser/ast.c 

clean:
	rm -f ndc_ast_print