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

		switch (cmd->args[0]) {
			case 0: {
				//End
				// Push the command to the end of the command list so all threads receive the command
				pthread_mutex_lock(&(cmdList->lock));
				push(cmdList, cmd);
				pthread_mutex_unlock(&(cmdList->lock));

				// End the thread
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
				int transactionAmt[numTransactions];
				int i;
				int invalid = 0;
				for (i = 0; i < numTransactions; i++) {
					// Grab all the account numbers from the odd numbered arguments
					accountIndices[i] = cmd->args[1 + 2 * i];
					if (accountIndices[i] > numAccounts
							|| accountIndices[i] <= 0) {
						// Invalid Account number
						sprintf(out, "%d INVALID ACCOUNT", cmd->id);
						// Mark the invalid flag
						invalid = 1;
						// Stop the for loop
						break;
					}
					// Grab all the transaction amounts too
					transactionAmt[i] = cmd->args[2 + 2 * i];
				}
				// Check for duplicate account numbers (prevents a thread from deadlocking itself)
				if (checkDuplicates(accountIndices, numTransactions)) {
					invalid = 1;
					sprintf(out, "%d DUPLICATE ACCOUNT", cmd->id);
				}
				if (!invalid) {
					// Sort the account indices and transactions so they are always locked/unlocked in the same order
					parallelSort(accountIndices, transactionAmt,
							numTransactions);

					// Lock all accounts
					for (i = 0; i < numTransactions; i++) {
						pthread_mutex_lock(&(locks[accountIndices[i]]));
					}
					// Check valid balances
					for (i = 0; i < numTransactions; i++) {
						if (read_account(accountIndices[i])
								+ transactionAmt[i] < 0) {
							invalid = 1;
							// Found invalid balance
							sprintf(out, "%d ISF %d", cmd->id,
									accountIndices[i]);
							break;
						}
					}
					if (!invalid) {
						// Write all the new balances
						for (i = 0; i < numTransactions; i++) {
							int newBalance = read_account(accountIndices[i])
									+ transactionAmt[i];
							write_account(accountIndices[i], newBalance);
						}
						sprintf(out, "%d OK", cmd->id);
					}
					// Unlock all accounts
					for (i = 0; i < numTransactions; i++) {
						pthread_mutex_unlock(&(locks[accountIndices[i]]));
					}
				}

			}

		}
		// Lock the file
		pthread_mutex_lock(&(locks[numAccounts]));
		// Write to the file
		writeToFile(outFile, out);

		gettimeofday(&exectime, NULL);

		sprintf(out, " TIME %ld.%06d %ld.%06d\n", cmd->time.tv_sec,
				(int)cmd->time.tv_usec, exectime.tv_sec,(int) exectime.tv_usec);
		writeToFile(outFile, out);
		// Unlock the file
		pthread_mutex_unlock(&(locks[numAccounts]));
		free(cmd->args);
		free(cmd);
	}

	return NULL;
}
