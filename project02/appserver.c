#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>

#include "splitArgs.h"
#include "Bank.h"
#include "utils.h"
#include "commandList.h"
#include "worker.h"

int main(int argc, char *argv[]) {
	//User input prompt variables
	char prompt[2] = ">";
	char input[300];
	char * inputArgs[30];

	// Grab all the arguments
	if (argc != 4) {
		printf("< Incorrect number of arguments!\n");
		exit(0);
	}
	int numWorkers = atoi(argv[1]);
	int numAccounts = atoi(argv[2]);
	char * outFile = argv[3];

	// Initialize Command queue
	CommandList * cmdList = listInit();

	// Initialize all the accouts and locks for said accounts
	if (initialize_accounts(numAccounts) == 0) {
		printf("< Initialization Error!");
		exit(0);
	}
	pthread_mutex_t * locks = (pthread_mutex_t *) malloc(
			sizeof(pthread_mutex_t[numAccounts]));

	// Initialize worker threads
	pthreadArgs * args = (pthreadArgs *) malloc(sizeof(pthreadArgs *));
	args->cmdListArg = cmdList;
	args->outFileArg = outFile;
	args->locksArg = locks;
	args->numAccounts = &numAccounts;

	pthread_t worker_tid[numWorkers];
	int thread_index[numWorkers];
	int i;
	for (i = 0; i < numWorkers; i++) {
		thread_index[i] = i;
		pthread_create(&worker_tid[i], NULL, worker, (void *) args);
	}

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

		// Increment ID
		id++;

		// Create a new command
		// Command will get freed when it is executed (in the thread)
		Command * cmd = commandInit(id, numArgs, inputArgs);

		//All of the checks for user input
		// END - exit normally, and shutdown all worker threads
		if (!(strcmp(inputArgs[0], "END"))) {
			cmd->args[0] = -1;
		}
		// CHECK - check a balance
		else if (!(strcmp(inputArgs[0], "CHECK"))) {
			if (numArgs < 2) {
				// OH NO A GOTO OH GOD WHYYYYYYYYY
				goto invalid;
			}
			cmd->args[0] = 100;
			printf("\nCHECK: %d\n", cmd->args[0]);
		}
		// TRANS - perform a transaction
		else if (!(strcmp(inputArgs[0], "TRANS"))) {
			cmd->args[0] = 2;
			// Check for more than 1 argument, and odd number of arguments
			// (account and value pair, plus the initial argument
			if (numArgs < 2 || numArgs % 2 == 0) {
				goto invalid;
			}
		}
		// Invalid command
		else {
			invalid:

			// Reset ID
			id--;
			// Discard command
			free(cmd);
			// Inform user of incorrect command
			printf("< Invalid command, commands are: END, CHECK, TRANS\n");
			continue;
		}

		// Push the command to the command list
		pthread_mutex_lock(&(cmdList->lock));
		push(cmdList, cmd);
		pthread_mutex_unlock(&(cmdList->lock));

		if (cmd->args[0] == -1) {
			printf("< Ending program\n");
			break;
		}

		// Print the ID
		printf("< ID %d CommandTypeC: %s, CommandTypeA: %d, CommandTypeB: %d ListSize: %d\n", cmd->id,inputArgs[0], cmd->args[0], cmdList->foot->args[0], cmdList->size);
	}

	// Wait for all worker threads to complete
	for (i = 0; i < numWorkers; i++) {
		pthread_join(worker_tid[i], NULL);
	}

	free(cmdList);
	free(locks);
	free(args);
	exit(1);
}
