#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define DEBUG 0

/**
 * Takes in a long string and a pointer to a string array, and splits the long string into smaller substrings
 * using space, tab, and newlines as delimeters.
 *
 * The function will recognize
 * This function also handles escape character behavior, in that an escape character "\" gets removed from the string,
 * and the character following it is ignored by standard processing (like spaces being used as a delimeter).
 */
int splitArgs(char *line, char **argv) {
	char *end = line + strlen(line);
	//Argument counter
	int count = 0;

	//The first argument points to the first character of the line
	*argv++ = line;
	count++;

	//Wait until the end of the line
	while (*line != '\0') {

		//Find spaces, tabs, or new lines, replace with null character
		if (*line == ' ' || *line == '\t' || *line == '\n') {
			*line++ = '\0';

			//If the current line pointer (character after space, tab, or newline)
			//is not a null character, we have a new arg
			if (*line != '\0') {
				//Increment the argv pointer, and point it at the beginning of the next line
				*argv++ = line;
				//Increase the argument counter
				count++;
			}
		}
	}

	//End of argument list
	*argv = '\0';

	return count;
}
