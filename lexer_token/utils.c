
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "utils.h"

bool switch_counter (int c) 
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
			return true;
		case '*' :
			return true;
		case '/' :
			return true;
		case '=' :
			return true;
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
			if (switch_counter (c)) {
				count++;
				c = fgetc (fcount);
			} 
		}
	}
	return count;
}