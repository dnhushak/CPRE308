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

	//Reset arg pointer to be back to the original passed in value
	argv = argv - count;
	for (int i = 0; i < count; i++) {
		//Check if first character is a $, indicating environment variable
		if (!strncmp(argv[i], "$", 1)) {
			//Get rid of the $ out front by moving the pointer up by 1
			argv[i] = argv[i] + 1;

			//Copy the contents of the environment variable
			if (getenv(argv[i])) {
				argv[i] = getenv(argv[i]);
				printf("%s\n", argv[i]);
			} else {
				printf("Environment variable %s is not set\n", argv[i]);
				return -1;
			}
		}
	}

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

		if (numArgs <0){
			printf("Invalid Argument(s)\n");
			continue;
		}

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
			if(getenv(inputArgs[1])){
			printf("Environment variable %s has value: %s\n", inputArgs[1],
					getenv(inputArgs[1]));}
			else{
				printf("Environment variable %s is not set\n", inputArgs[1]);
			}
		} else if (!(strcmp(inputArgs[0], "set"))) {
			printf("Environment variable %s has been set with value: %s\n",
					inputArgs[1], inputArgs[2]);
			setenv(inputArgs[1], inputArgs[2], 1);
		} else {
			execute(inputArgs, nowait);
		}

	}
}
