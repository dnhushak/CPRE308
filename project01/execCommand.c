#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

void printstatus(int status, int pid, char* arg) {
	if (WIFEXITED(status)) {
		printf("[%d] %s Exit %d\n", pid, arg, WEXITSTATUS(status));
	}
	if (WIFSIGNALED(status)) {
		printf("[%d] %s Killed (%d)\n", pid, arg, WTERMSIG(status));
	}
}

pid_t execute(char **argv, int nowait) {
	//Pid
	pid_t pid;
	int status;
	// Fork program, check for forking return value
	if ((pid = fork()) < 0) {
		printf("ERROR: Forking failed\n");
		//Exit fork (Doesn't exit parent process!)
		exit(1);
	} else if (pid == 0) {
		//Is a child process without a forking error.
		printf("[%d] %s\n", getpid(), argv[0]);

		//Execute the command, with the various arguments
		if (execvp(*argv, argv) < 0) {
			//Execution error
			printf("ERROR: Executable not found\n");
			exit(1);
		}
	}
	return pid;

}
