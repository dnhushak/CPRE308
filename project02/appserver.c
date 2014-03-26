#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splitArgs.h"
#include <sys/types.h>
#include <sys/time.h>
#include "writeToFile.h"
#include "Bank.h"
#include <fcntl.h>
#include <pthread.h>
#include "commandList.h"

typedef struct pthreadArgs {
	CommandList * cmdListArg;
	char * outFileArg;
	pthread_mutex_t * locksArg;
	int * numAccounts;
} pthreadArgs;

void * worker(void *);

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

		// Create a new command
		// Command will get freed when it is executed (in the thread)
		Command * cmd = (Command*) malloc(sizeof(Command*));

		// Increment ID
		id++;
		// Set the command ID
		cmd->id = id;
		// Set the command time
		gettimeofday(&cmd->time, NULL);
		// Copy over all the arguments
		cmd->args = (int *) malloc(sizeof(int[numArgs]));
		int i;
		for (i = 1; i < numArgs; i++) {
			cmd->args[i] = atoi(inputArgs[i]);
		}

		//All of the checks for user input
		// END - exit normally, and shutdown all worker threads
		if (!(strcmp(inputArgs[0], "END"))) {
			cmd->args[0] = 0;
			printf("< Ending program\n");
		}
		// CHECK - check a balance
		else if (!(strcmp(inputArgs[0], "CHECK"))) {
			cmd->args[0] = 1;
			if (numArgs < 2) {
				// OH NO A GOTO OH GOD WHYYYYYYYYY
				goto invalid;
			}
		}
		// TRANS - perform a transaction
		else if (!(strcmp(inputArgs[0], "TRANS"))) {
			cmd->args[0] = 2;
			// Check for more than 1 argument, and odd number of arguments
			// (account and value pair, plus the initial argument
			if (numArgs < 2 || numArgs % 2 != 0) {
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

		// Check for end
		if (cmd->args[0] == 0) {
			break;
		}

		// Print the ID
		printf("< ID %d\n", cmd->id);
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

void * worker(void * args) {
	// Get the arguments from the input struct
	pthreadArgs * pargs = args;
	CommandList * cmdList = pargs->cmdListArg;
	pthread_mutex_t * locks = pargs->locksArg;
	char * outFile = pargs->outFileArg;
	int numAccounts = *(pargs->numAccounts);
	// Output buffer to write to file
	char out[300];
	struct timeval exectime;

	while (1) {

		if (cmdList->size == 0) {
			usleep(10000);
			continue;
		}

		// Lock the command list
		pthread_mutex_lock(&(cmdList->lock));

		// Get the next command to execute
		Command * cmd = pop(cmdList);

		// Unlock the command list
		pthread_mutex_unlock(&(cmdList->lock));

		switch (cmd->args[0]) {
			case 0: {
				//End

				// Push the command to the end of the command list so all threads receive the command
				pthread_mutex_lock(&(cmdList->lock));
				push(cmdList, cmd);
				pthread_mutex_unlock(&(cmdList->lock));

				return NULL;
				break;
			}
			case 1: {
				// Balance Check
				// Get the account number
				int accountIndex = cmd->args[1];

				// Check for valid account number
				if (accountIndex <= numAccounts && accountIndex > 0) {

					// Get the account balance
					pthread_mutex_lock(&locks[accountIndex]);
					int balance = read_account(accountIndex);
					pthread_mutex_unlock(&locks[accountIndex]);
					sprintf(out, "%d BAL %d", cmd->id, balance);
				} else {
					// Invalid Account
					sprintf(out, "%d INVALID ACCOUNT", cmd->id);
				}
				// Write to the file
				writeToFile(outFile, out);
				break;
			}
			case 2: {
				// Transaction
			}

		}
		gettimeofday(&exectime, NULL);
		sprintf(out, "\r\t\t\t\tTIME %ld.%06d %ld.%06d\n", cmd->time.tv_sec,
				cmd->time.tv_usec, exectime.tv_sec, exectime.tv_usec);
		writeToFile(outFile, out);
		free(cmd);
	}

	return NULL;
}
