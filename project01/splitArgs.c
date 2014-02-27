#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int splitArgs(char *line, char **argv) {
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

			//If the current line pointer is not a null character, we have a new arg
			if (*line != '\0') {
				//Increment the argv pointer, and point it at the beginning of the next line
				*argv++ = line;
				count++;
			}
		}
		//Check for quoted args
		else if (*line == '"') {
			line++;
			while (*line++ != '"') {

			}
		}
		//TODO Fix escape character behavior
		//Check for escape character
		else if (*line == '\\') {
			//Skip over the escaped character
			line += 2;
		} else {
			//Character is nothing special, advance...
			line++;
		}
	}
	*argv = '\0';

	//Resolve environment variables to their actual values

	//Reset arg pointer to be back to the original passed in value
	argv = argv - count;
	for (int i = 0; i < count; i++) {
		//Check if first character is a $, indicating environment variable
		if (!strncmp(argv[i], "$", 1)) {
			//Get rid of the $ out front by moving the pointer up by 1
			argv[i] = argv[i] + 1;

			//See if environment variable is set
			if (getenv(argv[i])) {
				//If set, repoint argv[i] to the environment variable return value
				argv[i] = getenv(argv[i]);
			} else {
				//If not set, notify user and return with -1
				printf("Environment variable %s is not set\n", argv[i]);
				return -1;
			}
		}
	}

	return count;
}
