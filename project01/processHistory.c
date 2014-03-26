#include "processHistory.h"

void addToList(ProcessList * procList, Process * proc) {
	Process * newProc = (Process*) malloc(sizeof(Process));
	newProc = proc;
	newProc->next = NULL;

	if (procList->size == 0) {
		//Empty list
		newProc->prev = NULL;
		//Have the head point to the new process
		procList->head = newProc;
		//Foot also points to the new process
		procList->foot = procList->head;
		//Increment the size
		procList->size++;
	} else {
		//Place at the end of the list
		newProc->prev = procList->foot;
		newProc->prev->next = newProc;
		//Make the foot point to the new end of the list
		procList->foot = newProc;
		//Increment the size
		procList->size++;
	}
}

Process * removeFromList(ProcessList * procList, pid_t pid) {
	Process * dummy = procList->head;
	//Find the process with the pid
	//Iterate through the list to find it
	if (procList->size == 0) {
		//empty list, return null
		return NULL;
	}

	//Start at head of list
	dummy = procList->head;
	int i;
	for (i = 0; i < procList->size; i++) {
		if (dummy->pid == pid) {
			//We found it!
			//First, fix the list
			if (i == 0) {
				//Beginning of the list, point head to next node
				procList->head = dummy->next;
			}
			if (i == procList->size) {
				//End of list, point foot to previous node
				//Note that we could be at beginning and end
				//simultaneously, hence the lack of "else"
				procList->foot = dummy->prev;
			}
			//If there is a previous node, set it's next to the dummy's next
			if (dummy->next != NULL) {
				dummy->next->prev = dummy->prev;
			}

			//If there is a next node, set it's prev to the dummy's prev
			if (dummy->prev != NULL) {
				dummy->prev->next = dummy->next;
			}
			procList->size--;
			return dummy;
		} else {
			dummy = dummy->next;
		}
	}
	//Couldn't find it
	return NULL;
}

void printProcesses(ProcessList * procList) {
	if (procList->size == 0) {
		printf("No jobs running in background\n");
	} else {
		printf("%d processes are currently running:\n", procList->size);

		Process * dummy = procList->head;
		int i;
		for (i = 0; i < procList->size; i++) {
			printf("[%d] %s is currently running\n", dummy->pid,
					dummy->procname);
			dummy = dummy->next;
		}
	}
}

