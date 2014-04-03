/*******************************************************************************
 *
 * CprE 308 Scheduling Lab
 *
 * scheduling.c
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define NUM_PROCESSES 20
//#define DEBUG

struct process {
	/* Values initialized for each process */
	int arrivaltime; /* Time process arrives and wishes to start */
	int runtime; /* Time process requires to complete job */
	int priority; /* Priority of the process */

	/* Values algorithm may use to track processes */
	int starttime;
	int endtime;
	int flag;
	int remainingtime;

	// I added this
	int number;
};

/* Forward declarations of Scheduling algorithms */
void first_come_first_served(struct process *proc);
void shortest_remaining_time(struct process *proc);
void round_robin(struct process *proc);
void round_robin_priority(struct process *proc);
void sortProcessesByArrivalTime(struct process *proc);

int main() {
	int i;
	struct process proc[NUM_PROCESSES], /* List of processes */
	proc_copy[NUM_PROCESSES]; /* Backup copy of processes */

	/* Seed random number generator */
	/*srand(time(0));*//* Use this seed to test different scenarios */
	srand(0xC0FFEE); /* Used for test to be printed out */

	/* Initialize process structures */
	for (i = 0; i < NUM_PROCESSES; i++) {
		proc[i].arrivaltime = rand() % 100;
		proc[i].runtime = (rand() % 30) + 10;
		proc[i].priority = rand() % 3;
		proc[i].starttime = 0;
		proc[i].endtime = 0;
		proc[i].flag = 0;
		// I Changed this
		proc[i].remainingtime = proc[i].runtime;
		// I added this
		proc[i].number = i;
	}

	/* Show process values */
	printf("\n\nProcess\t|\tArrival\t|\tRuntime\t|\tPriority\n");
	for (i = 0; i < NUM_PROCESSES; i++)
		printf("%d\t|\t%d\t|\t%d\t|\t%d\n", proc[i].number, proc[i].arrivaltime,
				proc[i].runtime, proc[i].priority);

	/* Run scheduling algorithms */
	printf("\n\nFirst come first served\n");
	memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
	first_come_first_served(proc_copy);

	printf("\n\nShortest remaining time\n");
	memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
	shortest_remaining_time(proc_copy);

	printf("\n\nRound Robin\n");
	memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
	round_robin(proc_copy);

	printf("\n\nRound Robin with priority\n");
	memcpy(proc_copy, proc, NUM_PROCESSES * sizeof(struct process));
	round_robin_priority(proc_copy);

	return 0;
}

void first_come_first_served(struct process *proc) {
	sortProcessesByArrivalTime(proc);
	int procCnt = 0;
	int time = 0;
	int avTime = 0;
	int greatestTime = 0;
	struct process * currentProc;

	while (procCnt < NUM_PROCESSES) {
		// Set the current process to the count
		currentProc = &(proc[procCnt]);

		if (currentProc->arrivaltime <= time) {
			printf("Process %d started at time %d\n", proc[procCnt].number,
					time);
			// Process arrives at time
			currentProc->starttime = time;

			// While it still has time remaining...
			while (currentProc->remainingtime) {
#ifdef DEBUG
				printf(
						"\tOn process number %d, remaining time is %d, runtime is %d, current time is %d\n",
						currentProc->number, currentProc->remainingtime,
						currentProc->runtime, time);
#endif
				// Increment the time counter
				time++;
				// Decrement the remaining time counter
				currentProc->remainingtime--;
			}
			currentProc->endtime = time;
			printf("Process %d finished at time %d\n", currentProc->number,
					time);
			avTime += currentProc->endtime - currentProc->starttime;
			if ((currentProc->endtime - currentProc->starttime)
					> greatestTime) {
				greatestTime = avTime;
			}
			procCnt++;
		}
		else {
#ifdef DEBUG
			printf("\tNo process, current time is %d\n", time);
#endif
		time++;
		}
	}
	avTime /= NUM_PROCESSES;
	printf("Average time from arrival to finish is %d seconds\n", avTime);
	printf("Greatest time from arrival to finish is %d seconds\n",
			greatestTime);
}

void shortest_remaining_time(struct process *proc) {
	sortProcessesByArrivalTime(proc);
	int procCnt = 0;
	int time = 0;
	int avTime = 0;
	int greatestTime = 0;
	struct process * currentProc;

	// Start with the first process
	currentProc = &(proc[0]);

	// For each process...
	while (procCnt < NUM_PROCESSES) {
		// If the process has arrived but not ended
		if (currentProc->arrivaltime <= time && currentProc->endtime == 0) {
			newprocess:

			// Check if this process has started (endtime should not equal remaining time if it has)
			if (currentProc->runtime == currentProc->remainingtime) {
				printf("Process %d started at time %d\n", currentProc->number,
						time);
				// Process arrives at time
				currentProc->starttime = time;
			}

			// While it still has time remaining...
			while (currentProc->remainingtime) {
#ifdef DEBUG
				printf(
						"\tOn process number %d, remaining time is %d, runtime is %d, current time is %d\n",
						currentProc->number, currentProc->remainingtime,
						currentProc->runtime, time);
#endif
				/*
				 * This whole for loop is for preemtive checking..
				 * Wasn't sure if it was suppose to be preemtive or not
				 *
				 * To remove preemtive, simple comment out from here...
				 */
				int j;
				// Our comparison starts at infinity
				int shortest = INT_MAX;
				for (j = 0; j < NUM_PROCESSES; j++) {
					// Check if the process has arrived (arrivaltime <= time)
					// AND its remaining time is less than that of the current process (remainingtime<remainingtime)
					// AND it is not completed (remainingtime > 0)
					// AND it is less than our shortest found time
					if (proc[j].arrivaltime <= time
							&& proc[j].remainingtime
									< currentProc->remainingtime
							&& proc[j].remainingtime > 0
							&& proc[j].remainingtime < shortest) {
						// Switch process
						currentProc = &(proc[j]);
						shortest = currentProc->remainingtime;
#ifdef DEBUG
						printf(
								"\t\tSwitching to process number %d, remaining time is %d, runtime is %d, current time is %d\n",
								currentProc->number, currentProc->remainingtime,
								currentProc->runtime, time);
#endif
					}
					// If we found a shorter process
					if (shortest != INT_MAX) {
						// ZOMGS A GOTO STATEMENT WHAT WILL WE ALL DOOOOO
						goto newprocess;
					}
				}
				/*
				 * ... to here
				 */
				// Increment the time counter
				time++;
				// Decrement the remaining time counter
				currentProc->remainingtime--;
			}

			currentProc->endtime = time;
			printf("Process %d finished at time %d\n", currentProc->number,
					currentProc->endtime);
			avTime += currentProc->endtime - currentProc->starttime;
			if ((currentProc->endtime - currentProc->starttime)
					> greatestTime) {
				greatestTime = avTime;
			}
			procCnt++;
			// Find the next process to run
			int i;
			// Our comparison starts at infinity
			int shortest = INT_MAX;
			for (i = 0; i < NUM_PROCESSES; i++) {
				// Skip over processes that have not yet arrived and/or have completed
				if (proc[i].arrivaltime <= time && proc[i].remainingtime > 0) {
					// If the process we are looking at has a shorter remaining time than what we've found
					if (proc[i].remainingtime < shortest) {
						// Update the shortest time we've found
						shortest = proc[i].remainingtime;
						// Set the current process to it
						currentProc = &(proc[i]);
					}
				}
			}
		}

		else {
#ifdef DEBUG
			printf("\tNo process, current time is %d\n", time);
#endif
		time++;
		}
	}
	avTime /= NUM_PROCESSES;
	printf("Average time from arrival to finish is %d seconds\n", avTime);
	printf("Greatest time from arrival to finish is %d seconds\n",
			greatestTime);
}

void round_robin(struct process *proc) {
	sortProcessesByArrivalTime(proc);
	int procCnt = 0;
	int time = 0;
	int avTime = 0;
	int greatestTime = 0;
	struct process * currentProc;

	// Start with the first process
	currentProc = &(proc[0]);

	// For each process...
	while (procCnt < NUM_PROCESSES) {
		// If the process has arrived but not ended
		if (currentProc->arrivaltime <= time && currentProc->endtime == 0) {
			newprocess:

			// Check if this process has started (endtime should not equal remaining time if it has)
			if (currentProc->runtime == currentProc->remainingtime) {
				printf("Process %d started at time %d\n", currentProc->number,
						time);
				// Process arrives at time
				currentProc->starttime = time;
			}

			// While it still has time remaining...
			while (currentProc->remainingtime) {
#ifdef DEBUG
				printf(
						"\tOn process number %d, remaining time is %d, runtime is %d, current time is %d\n",
						currentProc->number, currentProc->remainingtime,
						currentProc->runtime, time);
#endif
				time++;
				// Decrement the remaining time counter
				currentProc->remainingtime--;
				int i;
				// Find our next process that hasn't finished
				for (i = 0; i < NUM_PROCESSES; i++) {
					if (proc[i].endtime == 0
							&& proc[i].number != currentProc->number) {
						currentProc = &(proc[i]);
						goto newprocess;
					}
				}
			}

			currentProc->endtime = time;
			printf("Process %d finished at time %d\n", currentProc->number,
					currentProc->endtime);
			avTime += currentProc->endtime - currentProc->starttime;
			if ((currentProc->endtime - currentProc->starttime)
					> greatestTime) {
				greatestTime = avTime;
			}
			procCnt++;
			int i;
			// Find our next process that hasn't finished
			for (i = 0; i < NUM_PROCESSES; i++) {
				if (proc[i].endtime == 0) {
					currentProc = &(proc[i]);
					goto newprocess;
				}
			}
		}

		else {
#ifdef DEBUG
			printf("\tNo process, current time is %d\n", time);
#endif
		time++;
		}
	}
	avTime /= NUM_PROCESSES;
	printf("Average time from arrival to finish is %d seconds\n", avTime);
	printf("Greatest time from arrival to finish is %d seconds\n",
			greatestTime);
}

void round_robin_priority(struct process *proc) {
	sortProcessesByArrivalTime(proc);
	int procCnt = 0;
	int time = 0;
	int avTime = 0;
	int greatestTime = 0;
	struct process * currentProc;

	// Start with the first process
	currentProc = &(proc[0]);

	// For each process...
	while (procCnt < NUM_PROCESSES) {
		// If the process has arrived but not ended
		if (currentProc->arrivaltime <= time && currentProc->endtime == 0) {
			newprocess:

			// Check if this process has started (endtime should not equal remaining time if it has)
			if (currentProc->runtime == currentProc->remainingtime) {
				printf("Process %d started at time %d\n", currentProc->number,
						time);
				// Process arrives at time
				currentProc->starttime = time;
			}

			// While it still has time remaining...
			while (currentProc->remainingtime) {
#ifdef DEBUG
				printf(
						"\tOn process number %d, priority is %d, remaining time is %d, runtime is %d, current time is %d\n",
						currentProc->number, currentProc->priority,
						currentProc->remainingtime, currentProc->runtime, time);
#endif
				time++;
				// Decrement the remaining time counter
				currentProc->remainingtime--;
				int i;
				int flag = 0;
				int number = currentProc->number;
				int highestPrioFound = currentProc->priority;
				// Find our next process that hasn't finished and has higher or equal priority
				for (i = 0; i < NUM_PROCESSES; i++) {
					if (proc[i].arrivaltime <= time && proc[i].endtime == 0
							&& proc[i].priority >= highestPrioFound
							&& proc[i].number != number) {
						currentProc = &(proc[i]);
						highestPrioFound = currentProc->priority;
						flag = 1;
					}
				}
				if (flag) {
					goto newprocess;
				}

			}

			currentProc->endtime = time;
			printf("Process %d finished at time %d\n", currentProc->number,
					currentProc->endtime);
			avTime += currentProc->endtime - currentProc->starttime;
			if ((currentProc->endtime - currentProc->starttime)
					> greatestTime) {
				greatestTime = (currentProc->endtime - currentProc->starttime);
			}
			procCnt++;
			int i;
			int highestPrioFound = 0;
			// Find our next process that hasn't finished and has higher or equal priority
			for (i = 0; i < NUM_PROCESSES; i++) {
				if (currentProc->arrivaltime <= time && proc[i].endtime == 0
						&& proc[i].priority >= highestPrioFound) {
					currentProc = &(proc[i]);
					highestPrioFound = currentProc->priority;
				}
			}
		}

		else {
#ifdef DEBUG
			printf("\tNo process, current time is %d\n", time);
#endif
		time++;
		}
	}
	avTime /= NUM_PROCESSES;
	printf("Average time from arrival to finish is %d seconds\n", avTime);
	printf("Greatest time from arrival to finish is %d seconds\n",
			greatestTime);
}

// Sorts the processes in order by arrival time (simple bubble sort)
void sortProcessesByArrivalTime(struct process *proc) {
	int c, d;
	struct process * swap = malloc(sizeof(struct process));
// Iterate through every process
	for (c = 0; c < (NUM_PROCESSES - 1); c++) {
		// For every process after this...
		for (d = 0; d < NUM_PROCESSES - c - 1; d++) {
			// If its arrival time is later than the next process...
			if (proc[d].arrivaltime > proc[d + 1].arrivaltime) {
				//...Swap the processes
				memcpy(swap, &(proc[d]), sizeof(struct process));
				memcpy(&(proc[d]), &(proc[d + 1]), sizeof(struct process));
				memcpy(&(proc[d + 1]), swap, sizeof(struct process));
			}
		}
	}
	free(swap);
}

