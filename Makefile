CC = gcc
CFLAGS = -Wall -g -std=c99 -DDEBUG


all: main.c 
	$(CC) $(CFLAGS) -o ndc_ast_print_SA1 \
	main.c \
	utils.c \
	tokenizer/tokenize.c \
	parser/parser.c \
	parser/ast_util.c \
	parser/ast.c 

clean:
	rm -f ndc_ast_print
