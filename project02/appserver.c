#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splitArgs.h"
#include <sys/types.h>
#include "writeToFile.h"
#include "Bank.h"
#include <fcntl.h>
#include <pthread.h>

void * worker();

int main(int argc, char *argv[]) {

	//User input prompt variables
	char prompt[2] = ">";
	char input[300];
	char *inputArgs[30];

	// Grab all the arguments
	if (argc != 4) {
		printf("< Incorrect number of arguments!\n");
		exit(0);
	}
	int numWorkers = atoi(argv[1]);
	int numAccounts = atoi(argv[2]);
	char * outFile = argv[3];

	// Initialize worker threads
	pthread_t worker_tid[numWorkers];
	int thread_index[numWorkers];
	int i;
	for (i = 0; i < numWorkers; i++) {
		thread_index[i] = i;
		pthread_create(&worker_tid[i], NULL, worker, (void *) &thread_index[i]);
	}

	// Initialize all the accouts and locks for said accounts
	if (initialize_accounts(numAccounts) == 0) {
		printf("< Initialization Error!");
		exit(0);
	}
	pthread_mutex_t locks[numAccounts];
	int id = 0;

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
			break;
		}
		//pid - print the process ID
		else if (!(strcmp(inputArgs[0], "TRANS"))) {
			id++;
			printf("< ID %d\n", id);
//			writeToFile(outFile, "TRANSTEST\n");
		}
		//ppid - print the parent's process ID
		else if (!(strcmp(inputArgs[0], "CHECK"))) {
			id++;
			printf("< ID %d\n", id);
		} else {
			printf("< Invalid command\n");
			continue;
		}
	}

	// Wait for all worker threads to complete
	for (i = 0; i < numWorkers; i++) {
		pthread_join(worker_tid[i], NULL);
	}
	exit(1);
}

void * worker() {
	return NULL;
}
