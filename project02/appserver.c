#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splitArgs.h"
#include "execCommand.h"
#include <sys/types.h>
#include "processHistory.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {

	//User input prompt variables
	char prompt[2] = ">";
	char input[300];
	char *inputArgs[30];

	int numWorkers;
	int	numAccounts;
	char * outFile;

	if (argc !=4 ){
		printf("< Incorrect number of arguments!\n");
		exit(0);
	}

	numWorkers = atoi(argv[1]);
	numAccounts = atoi(argv[2]);
	outFile = argv[3];




	//Main loop
	while (1) {

		//Zero out the input buffer
		memset(&input, '\0', sizeof(input));

		//Display the shell prompt
		printf("%s ", prompt);

		//Grab input from user prompt
		fgets(input, sizeof(input), stdin);

		//Split the input line into an array of char arrays (string array)
		int numArgs;
		numArgs = splitArgs(input, inputArgs);

		//Check if a valid number of arguments have been set
		if (numArgs < 0) {
			printf("< Invalid Argument(s)\n");
			continue;
		}

		//Prevent execute from attempting to execute nothing (Simply pressing enter)
		if (numArgs == 1 && !strcmp(inputArgs[0], "\0")) {
			continue;
		}

//		int stdout_old = dup(1);
//		int fd = 1;
//		if (numArgs > 2 && !(strcmp(inputArgs[numArgs - 2], ">"))) {
//			//Open the file
//			fd = open(inputArgs[numArgs - 1], O_WRONLY | O_APPEND | O_CREAT,
//			S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
//
//			//Redirect standard out
//			if (dup2(fd, 1) < 0 || fd < 0) {
//				exit(-1);
//			}
//			//Zero these arguments
//			memset(&(inputArgs[numArgs - 1]), '\0',
//					sizeof(inputArgs[numArgs - 1]));
//			memset(&(inputArgs[numArgs - 2]), '\0',
//					sizeof(inputArgs[numArgs - 2]));
//			numArgs -= 2;
//		}

		//All of the checks for user input
		//exit - exit normally
		if (!(strcmp(inputArgs[0], "END"))) {
			exit(2);
		}
		//pid - print the process ID
		else if (!(strcmp(inputArgs[0], "TRANS"))) {
			printf("Process id is: [%d]\n", getpid());
		}
		//ppid - print the parent's process ID
		else if (!(strcmp(inputArgs[0], "CHECK"))) {
			printf("Parent Process id is: [%d]\n", getppid());
		} else {
			printf("< Invalid command\n");
		}

//		//Route stdout to stdout
//		//(In case previous command was piped to a file)
//		//close previous file descriptor
//		close(fd);
//		//Flush standard out
//		fflush(stdout);
//		//Redirect file stream to old standard out
//		if (dup2(stdout_old, 1) < 0) {
//			exit(-1);
//		}
//		//Close the saved old standard out file descriptor
//		close(stdout_old);

	}
}
