#pragma once
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "processHistory.h"

void printstatus(int status, int pid, char* arg);
void execute(char **argv, Process * executed);
