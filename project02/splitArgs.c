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
		//Check for quoted args
		else if (*line == '"' || *line == '\'') {
			//Keep track of double or single quote
			char quote = *line;
			// Advance the line, skipping over the quote
			//Replace quote with null character for clean practice
			*line++ = '\0';

			//Set the pointer of the last argument (which was set once we detected a space)
			//to the current character (1 after the quote)
			*(argv - 1) = line;

			//Advance until we find another quote or reach the end of the original input string
			while (*line++ != quote && line != end) {
				//Skip over everything in between quotes
			}
			if (line == end) {
				//We reached the end of the input line without
				//finding the close quote
				printf("Invalid argument: unclosed quote\n");
				//Reject the argument
				argv--;
				count--;
			} else {
				//Remove the end quote
				*(line - 1) = '\0';
			}
		}

		//Check for escape character
		else if (*line == '\\') {
			//Move the rest of the line left by one (removing escape character)
			memmove(line, line + 1, end - line);
			//Advance line pointer
			line++;
		} else {
			//Character is nothing special, advance...
			line++;
		}
	}

	//End of argument list
	*argv = '\0';

	//Resolve environment variables to their actual values

	//Reset arg pointer to be back to the original passed in value
	argv = argv - count;
	int i;
	for (i = 0; i < count; i++) {
		//Check if first character is a $, indicating environment variable
		if (!strncmp(argv[i], "$", 1)) {
			//Get rid of the $ out front by moving the pointer up by 1
			//and setting the $ to a null character
			*argv[i]++ = '\0';

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
#if DEBUG == 1
		//Print each argument
		printf("%s\n", argv[i]);
#endif
	}

	return count;
}
