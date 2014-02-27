#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splitArgs.h"
#include "execCommand.h"
#include <sys/types.h>

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
	//Get all optional arguments using the getopt function
	while ((ch = getopt(argc, argv, "p:")) != EOF) {
		switch (ch) {
		//Prompt argument
		case 'p':
			strcpy(prompt, optarg);
			break;
		}

	}

	//Main loop
	while (1) {
		memset(&input, '\0', sizeof(input));
		//Display the shell prompt
		printf("%s ", prompt);

		//Grab input from user prompt
		fgets(input, 300, stdin);

		//Split the input line into an array of char arrays (string array)
		int numArgs;
		numArgs = splitArgs(input, inputArgs);

		//Check if a valid number of arguments have been set
		if (numArgs < 0) {
			printf("Invalid Argument(s)\n");
			continue;
		}

		//Prevent execute from attempting to execute nothing (Simply pressing enter)
		if (numArgs == 1 && !strcmp(inputArgs[0], "\0")) {
			continue;
		}

		//Check for ampersand
		if (!(strcmp(inputArgs[numArgs - 1], "&"))) {
			nowait = 1;
		} else {
			nowait = 0;
		}

		//All of the checks for user input
		//exit - exit normally
		if (!(strcmp(inputArgs[0], "exit"))) {
			exit(2);
		}
		//pid - print the process ID
		else if (!(strcmp(inputArgs[0], "pid"))) {
			printf("Process id is: [%d]\n", getpid());
		}
		//ppid - print the parent's process ID
		else if (!(strcmp(inputArgs[0], "ppid"))) {
			printf("Parent Process id is: [%d]\n", getppid());
		}
		//pwd - print the parent's process ID
		else if (!(strcmp(inputArgs[0], "pwd"))) {
			char * buf;
			printf("Present Wording Directory is: %s\n", getwd(buf));
		}
		//cd - change current directory
		else if (!(strcmp(inputArgs[0], "cd"))) {
			chdir(inputArgs[1]);
			char * buf;
			printf("Present Wording Directory is: %s\n", getwd(buf));
		}
		//get - get an environment variable
		else if (!(strcmp(inputArgs[0], "get"))) {

			//Check if variable is set
			if (getenv(inputArgs[1])) {
				//If it is set, print it.
				printf("Environment variable %s has value: %s\n", inputArgs[1],
						getenv(inputArgs[1]));
			} else {
				//If not set, notify user
				printf("Environment variable %s is not set\n", inputArgs[1]);
			}
		} else if (!(strcmp(inputArgs[0], "set"))) {
			//Set environment variable and notify user what has been set to what
			setenv(inputArgs[1], inputArgs[2], 1);
			printf("Environment variable %s has been set with value: %s\n",
					inputArgs[1], inputArgs[2]);
		} else {
			//If none of the built in functions, search the PATH for an executable
			execute(inputArgs, nowait);
		}

	}
}
