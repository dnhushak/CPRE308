#include "execCommand.h"

void printstatus(int status, int pid, char* arg) {
	if (WIFEXITED(status)) {
		printf("[%d] %s Exit %d\n", pid, arg, WEXITSTATUS(status));
	}
	if (WIFSIGNALED(status)) {
		printf("[%d] %s Killed (%d)\n", pid, arg, WTERMSIG(status));
	}
}

void execute(char **argv, Process * executed) {
	//Pid
	pid_t pid;
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
	} else {
		executed->procname = malloc(strlen(argv[0]));
		memcpy((executed->procname), argv[0], strlen(argv[0]));
		executed->pid = pid;
	}

}
