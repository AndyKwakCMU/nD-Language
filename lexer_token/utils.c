
#include <stdio.h>

#include "utils.h"

int word_counter (FILE* fcount)
{
        int count = 0;
	int c;
        
	while ((c = fgetc (fcount)) != EOF) {
		if (c == '#') {
			while (c != '\n' && c != EOF) {
				c = fgetc (fcount);
			}
			continue;
		}
		if (isspace (c)) {
			continue;
		}
		if (isalpha (c) || c == '_') {
			while ((c = fgetc (fcount)) != EOF &&
                                (isalnum (c) || c =='_')) 
			continue;
		}
		if (isdigit (c)) {
			while ((c = fgetc (fcount)) != EOF && 
                                isdigit (c)) 
			continue;
                }
		count++;
	}
	return count;
}