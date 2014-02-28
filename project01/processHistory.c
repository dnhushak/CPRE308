#include "processHistory.h"

void addToList(ProcessList * procList, pid_t pid, char* pname) {
	Process newProc;
	newProc.pid = pid;
	newProc.procname = pname;
	newProc.next = NULL;

	if (procList->size == 0) {
		//Empty list
		newProc.prev = NULL;
		//Have the head point to the new process
		procList->head = &newProc;
		//Foot also points to the new process
		procList->foot = &newProc;
		//Increment the size
		procList->size++;
	} else {
		//Place at the end of the list
		newProc.prev = procList->foot;
		//Make the foot point to the new end of the list
		procList->foot = &newProc;
		//Increment the size
		procList->size++;
	}
}

Process * removeFromList(ProcessList * procList, pid_t pid) {
	Process * dummy = procList->head;
	//Find the process with the pid
	while (dummy->pid != pid) {
		if (dummy->next == NULL) {
			//Not found, return -1
			return NULL;
		} else {
			//Advance down the list
			dummy = dummy->next;
		}
	}
	//Remove the item from the list
	dummy->prev->next = dummy->next;
	dummy->next->prev = dummy->prev;
	//Decrement the size
	procList->size--;
	//We've either found the node or returned null, return the process
	return dummy;
}

void printProcesses() {
	/*if (procList->size == 0) {
		printf("No jobs running in background\n");
	} else {
		printf("Processes currently running:\n");

		Process * dummy = procList->head;
		for (int i = 0; i < procList->size; i++) {
			printf("[%d] %s", dummy->pid, dummy->procname);
			dummy = dummy->next;
		}
	}*/
	printf("Process List\n");
}

