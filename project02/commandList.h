#pragma once
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct Command {
	int id;
	int * args;
	int numArgs;
	struct timeval time;
	struct Command * next;
	struct Command * prev;
} Command;

typedef struct CommandList {
	pthread_mutex_t lock;
	int size;
	Command * head;
	Command * foot;
} CommandList;

void push(CommandList *, Command *);
struct Command * pop(CommandList *);
CommandList * listInit();
Command * commandInit(int, int, char * *);
