#pragma once
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Process {
	char * procname;
	pid_t pid;
	struct Process * next;
	struct Process * prev;
} Process;

typedef struct ProcessList {
	int size;
	Process * head;
	Process * foot;
} ProcessList;

void addToList(ProcessList * pTable, Process * proc);
struct Process * removeFromList(ProcessList * procList, pid_t pid);
void printProcesses(ProcessList * procList);
