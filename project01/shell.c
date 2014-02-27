#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include  <sys/types.h>

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
		} else {
			line++; /* skip the argument until ...    */
		}
	}
	*argv = '\0';
	return count;
}

void execute(char **argv, int nowait) {

	pid_t pid;
	int status;

	if ((pid = fork()) < 0) { /* fork a child process           */
		printf("ERROR: Forking failed\n");
		exit(1);
	} else if (pid == 0) { /* for the child process:         */
		printf("Process %s executing with process ID: %d\n", argv[0], getpid());
		if (execvp(*argv, argv) < 0) { /* execute the command  */
			printf("ERROR: Executable not found\n");
			exit(1);
		}
	} else {
		if (!nowait) {
			while (wait(&status) != pid)
				;
		} else {

			printf("Process executing in background\n");
		}
	}
}

int main(int argc, char *argv[]) {

	//User input prompt variables
	char prompt[20] = "308sh>";
	char input[300];
	char *inputArgs[30];
	int nowait;

	//Scans for argument inputs
	extern char *optarg;
	extern int optind, opterr;
	int ch;
	while ((ch = getopt(argc, argv, "p:")) != EOF) {
		switch (ch) {
		case 'p':
			strcpy(prompt, optarg);
			break;
		}

	}
	while (1) {
		memset(&input, '\0', sizeof(input));
		//Display the shell prompt
		printf("%s ", prompt);

		//Grab input from user prompt
		fgets(input, 300, stdin);

		//Split the input line into an array of char arrays (string array)
		int numArgs;
		numArgs = splitArgs(input, inputArgs);

		//Check for ampersand
		if (!(strcmp(inputArgs[numArgs - 1], "&"))) {
			nowait = 1;
		} else {
			nowait = 0;
		}

		//All of the checks for user input
		if (!(strcmp(inputArgs[0], "exit"))) {
			return 0;
		} else if (!(strcmp(inputArgs[0], "pid"))) {
			printf("Process id is: [%d]\n", getpid());
		} else if (!(strcmp(inputArgs[0], "ppid"))) {
			printf("Parent Process id is: [%d]\n", getppid());
		} else if (!(strcmp(inputArgs[0], "get"))) {
			printf("Environment variable %s has value: %s", inputArgs[1],
					getenv(inputArgs[1]));
		} else {
			execute(inputArgs, nowait);
		}

	}
}
