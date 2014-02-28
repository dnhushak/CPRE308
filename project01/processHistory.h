#define PROCESS_HISTORY 100
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

struct ProcessList;
struct Process;

void addToList(struct ProcessList * pTable, pid_t pid, char* pname);
struct Process * removeFromList(struct ProcessList * procList, pid_t pid);
void printProcesses();

typedef struct Process {
	char * procname;
	int pid;
	struct Process * next;
	struct Process * prev;
} Process;

typedef struct ProcessList {
	int size;
	Process * head;
	Process * foot;
	void (*printProcesses)();
} ProcessList;

