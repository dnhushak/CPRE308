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
#include "commandList.h"

typedef struct pthreadArgs {
	CommandList * cmdListArg;
	char * outFileArg;
	pthread_mutex_t * locksArg;
	int * endArg;
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
	CommandList * cmdList = (CommandList*) malloc(sizeof(CommandList*));
	cmdList->size = 0;
	cmdList->head = cmdList->foot = NULL;
	printf("1 %d\n", cmdList->size);
	int end = 0;

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
	args->endArg = &end;

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

		printf("< %s\n", input);
		//All of the checks for user input
		//exit - exit normally
		if (!(strcmp(inputArgs[0], "END"))) {
			printf("< Ending program\n");
			end = 1;
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

void * worker(void * args) {
	// Get the arguments from the input struct
	pthreadArgs * pargs = args;
	CommandList * cmdList = pargs->cmdListArg;
	pthread_mutex_t * locks = pargs->locksArg;
	char * outFile = pargs->outFileArg;
	int * end = pargs->endArg;

	// Wait until there is a command to execute
	while (*end != 1 || cmdList->size != 0) {

		//printf("End: %d Command List Size: %d\n", *end, cmdList->size);

		if (cmdList->size == 0) {
			//printf("asdf\n");
			sleep(1);
			continue;
		}
//		// Lock the command list
//		pthread_mutex_lock(&cmdList->lock);
//
//		// Get the next command to execute
//		Command * cmd = pop(cmdList);
//
//		// Unlock the command list
//		pthread_mutex_unlock(&cmdList->lock);
//
//		if (cmd->commandType == 0) {
//			char out[300];
//			int accountIndex = atoi(cmd->args[1]);
//
//			pthread_mutex_lock(&locks[accountIndex]);
//			int balance = read_account(accountIndex);
//			pthread_mutex_unlock(&locks[accountIndex]);
//
//			sprintf(out, "%d BAL %d", cmd->id, balance);
//			// Balance check
//			writeToFile(outFile, out);
//
//		}
//		free(cmd);
	}

	return NULL;
}
