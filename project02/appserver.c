#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splitArgs.h"
#include <sys/types.h>
#include "writeToFile.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {

	//User input prompt variables
	char prompt[2] = ">";
	char input[300];
	char *inputArgs[30];

	int numWorkers;
	int	numAccounts;
	char * outFile;

	// Grab all the arguments
	if (argc !=4 ){
		printf("< Incorrect number of arguments!\n");
		exit(0);
	}
	numWorkers = atoi(argv[1]);
	numAccounts = atoi(argv[2]);
	outFile = argv[3];

	// Initialize all the accouts
	initialize_accounts(numAccounts);


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



		//All of the checks for user input
		//exit - exit normally
		if (!(strcmp(inputArgs[0], "END"))) {
			exit(2);
		}
		//pid - print the process ID
		else if (!(strcmp(inputArgs[0], "TRANS"))) {
			printf("Process id is: [%d]\n", getpid());
			writeToFile(outFile, "TRANSTEST");
		}
		//ppid - print the parent's process ID
		else if (!(strcmp(inputArgs[0], "CHECK"))) {
			printf("Parent Process id is: [%d]\n", getppid());
		} else {
			printf("< Invalid command\n");
		}



	}
}
