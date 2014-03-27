#include "commandList.h"

void push(CommandList * cmdList, Command * cmd) {
	if (cmdList->size == 0) {
		//Empty list

		//Have the head point to the new process
		cmdList->head = cmd;
		//Foot also points to the new process
		cmdList->foot = cmdList->head;
	} else {
		//Place at the end of the list
		cmd->prev = cmdList->foot;
		cmd->prev->next = cmd;
		//Make the foot point to the new end of the list
		cmdList->foot = cmd;
	}
	//Increment the size
	cmdList->size++;
	cmd->next = NULL;
}

Command * pop(CommandList * cmdList) {
	if (cmdList->size == 0) {
		//empty list, return null
		return NULL;
	}

	// Grab the command to pop
	Command * cmd = cmdList->head;

	// Decrement command list size
	cmdList->size--;

	if (cmdList->size == 0) {
		//List is now empty
		cmdList->head = NULL;
		cmdList->foot = NULL;
	} else {
		// Point the head to the next in line
		cmdList->head = cmdList->head->next;

		// Remove the backwards link from the new first
		cmdList->head->prev = NULL;
	}
	return cmd;
}

CommandList * listInit() {
	CommandList * cmdList = (CommandList*) malloc(sizeof(CommandList));
	cmdList->size = 0;
	cmdList->head = cmdList->foot = NULL;
	pthread_mutex_init(&(cmdList->lock), NULL);
	return cmdList;
}

Command * commandInit(int id, int numArgs, char ** inputArgs) {
	Command * cmd = (Command*) malloc(sizeof(Command));

	cmd->next = NULL;
	cmd->prev = NULL;

	// Set the command ID
	cmd->id = id;
	// Set the command time
	gettimeofday(&(cmd->time), NULL);
	// Copy over all the arguments
	cmd->numArgs = numArgs;
	cmd->args = (int *) malloc(sizeof(int) * numArgs);
	//memset(&(cmd->args), '0', sizeof(int) * numArgs);
	int j;
	for (j = 1; j < numArgs; j++) {
		cmd->args[j] = atoi(inputArgs[j]);
	}
	return cmd;
}

