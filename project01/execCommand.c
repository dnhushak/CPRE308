#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void execute(char **argv, int nowait) {
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
	} else {
		//TODO Print the exit status
		//Is the parent process
		if (!nowait) {
			//If nowait flag is not set, wait until child process completes
			while (wait(&status) != pid)
				;
		} else {
			//TODO Improve the Ampersand wait function
			//Or, if nowait, then process in the background
			printf("Process executing in background\n");
		}
	}
}
