#pragma once
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Command {
	int commandType;
	int id;
	char * * args;
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
