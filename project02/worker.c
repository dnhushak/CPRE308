#include "worker.h"

void * worker(void * args) {
	// Get the arguments from the input struct
	pthreadArgs * pargs = args;
	CommandList * cmdList = pargs->cmdListArg;
	pthread_mutex_t * locks = pargs->locksArg;
	char * outFile = pargs->outFileArg;
	int numAccounts = *(pargs->numAccounts);

	// Output buffer to write to file
	char out[300];
	// Time struct to store executed time
	struct timeval exectime;

	while (1) {

		// Lock the command list
		pthread_mutex_lock(&(cmdList->lock));
		if (cmdList->size == 0) {

			pthread_mutex_unlock(&(cmdList->lock));
			usleep(10000);
			continue;
		}

		// Get the next command to execute
		Command * cmd = pop(cmdList);

		// Unlock the command list
		pthread_mutex_unlock(&(cmdList->lock));

		printf("===Command ID: %d\n", cmd->args[0]);
		switch (cmd->args[0]) {
			case -1: {
				//End
				printf("Killing worker thread...\n");
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

				// Compute number of transactions
				// Each transaction has a pair of arguments so divide by two
				// The command is also in the number of arguments, hence - 1
				int numTransactions = (cmd->numArgs - 1) / 2;

				// Nicer way to access the account numbers
				int accountIndices[numTransactions];
				int i;
				for (i = 0; i < numTransactions; i++) {
					// Grab all the account numbers from the odd numbered arguments
					accountIndices[i] = cmd->args[1 + 2 * i];
					printf("PRE : %d: %d \n", i, accountIndices[i]);
				}
				sort(accountIndices, numTransactions);
				for (i = 0; i < numTransactions; i++) {
					printf("POST: %d: %d \n", i, accountIndices[i]);
				}
			}

		}
		gettimeofday(&exectime, NULL);
		sprintf(out, "\r\t\t\t\tTIME %ld.%06d %ld.%06d\n", cmd->time.tv_sec,
				cmd->time.tv_usec, exectime.tv_sec, exectime.tv_usec);
		writeToFile(outFile, out);

		//free(cmd->args);
		free(cmd);
	}

	return NULL;
}
