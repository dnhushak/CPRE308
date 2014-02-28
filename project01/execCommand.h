#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printstatus(int status, int pid, char* arg);
pid_t execute(char **argv, int nowait);
