#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splitArgs.h"
#include "execCommand.h"
#include <sys/types.h>
#include "processHistory.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {

	//User input prompt variables
	char prompt[20] = "308sh>";
	char input[300];
	char *inputArgs[30];
	int nowait;
	char buf[600];
	ProcessList * pList = (ProcessList*) malloc(sizeof(ProcessList));
	pList->size = 0;

	//Scans for argument inputs
	extern char *optarg;
	int ch;
	//Get all optional arguments using the getopt function
	while ((ch = getopt(argc, argv, "p:")) != EOF) {
		switch (ch) {
		//Prompt argument
		case 'p':
			strcpy(prompt, optarg);
			break;
		}

	}

	//Main loop
	while (1) {
		int status = 0;

		//Zero out the input buffer
		memset(&input, '\0', sizeof(input));

		//Display the shell prompt
		printf("%s ", prompt);

		//Grab input from user prompt
		fgets(input, sizeof(input), stdin);

		//Check for status change of background child processes
		pid_t pid = waitpid(-1, &status, WNOHANG);
		if (pid > 0) {
			//Remove from process list
			Process * ended = removeFromList(pList, pid);
			if (ended != NULL) {
				printstatus(status, ended->pid, ended->procname);
			}
		}

		//Split the input line into an array of char arrays (string array)
		int numArgs;
		numArgs = splitArgs(input, inputArgs);

		//Check if a valid number of arguments have been set
		if (numArgs < 0) {
			printf("Invalid Argument(s)\n");
			continue;
		}

		//Prevent execute from attempting to execute nothing (Simply pressing enter)
		if (numArgs == 1 && !strcmp(inputArgs[0], "\0")) {
			continue;
		}

		//Check for ampersand
		if (!(strcmp(inputArgs[numArgs - 1], "&"))) {
			//Set the nowait flag
			nowait = 1;
			//Remove the ampersand from the list of arguments sent to execute
			inputArgs[numArgs - 1] = '\0';
			numArgs--;
		} else {
			nowait = 0;
		}

		int stdout_old = dup(1);
		int fd = 1;
		//Check for file redirect, needs at least 3 arguments:
		// "CMD", the file redirect ">" and the file to redirect to "FILE"
		// CMD > FILE
		if (numArgs > 2 && !(strcmp(inputArgs[numArgs - 2], ">"))) {
			//Open the file
			fd = open(inputArgs[numArgs - 1], O_WRONLY | O_APPEND | O_CREAT,
			S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

			//Redirect standard out
			if (dup2(fd, 1) < 0 || fd < 0) {
				exit(-1);
			}
			//Zero these arguments
			memset(&(inputArgs[numArgs - 1]), '\0',
					sizeof(inputArgs[numArgs - 1]));
			memset(&(inputArgs[numArgs - 2]), '\0',
					sizeof(inputArgs[numArgs - 2]));
			numArgs -= 2;
		}

		//All of the checks for user input
		//exit - exit normally
		if (!(strcmp(inputArgs[0], "exit"))) {
			exit(2);
		}
		//pid - print the process ID
		else if (!(strcmp(inputArgs[0], "pid"))) {
			printf("Process id is: [%d]\n", getpid());
		}
		//ppid - print the parent's process ID
		else if (!(strcmp(inputArgs[0], "ppid"))) {
			printf("Parent Process id is: [%d]\n", getppid());
		}
		//pwd - print the parent's process ID
		else if (!(strcmp(inputArgs[0], "pwd"))) {
			printf("Present Wording Directory is: %s\n", getcwd(buf, sizeof(buf)));
		}
		//cd - change current directory
		else if (!(strcmp(inputArgs[0], "cd"))) {
			if (numArgs == 1 || !(strcmp(inputArgs[1], "~"))) {
				chdir(getenv("HOME"));
			} else {
				chdir(inputArgs[1]);
			}
			printf("Present Wording Directory is: %s\n", getwd(buf));
		}
		//get - get an environment variable
		else if (!(strcmp(inputArgs[0], "get"))) {
			if (numArgs > 1) {
				//Check if variable is set
				if (getenv(inputArgs[1])) {
					//If it is set, print it.
					printf("Environment variable %s has value: %s\n",
							inputArgs[1], getenv(inputArgs[1]));
				} else {
					//If not set, notify user
					printf("Environment variable %s is not set\n",
							inputArgs[1]);
				}
			} else {
				printf("No environment varialbe given\n");
			}
		} else if (!(strcmp(inputArgs[0], "set"))) {
			if (numArgs > 2) {
				//Set environment variable and notify user what has been set to what
				if (!setenv(inputArgs[1], inputArgs[2], 1)) {
					printf(
							"Environment variable %s has been set with value: %s\n",
							inputArgs[1], inputArgs[2]);
				}
			} else if (numArgs == 2) {
				if (!unsetenv(inputArgs[1])) {
					printf("Environment variable %s has been unset\n",
							inputArgs[1]);
				}
			} else {
				printf("No environment varialbe given\n");
			}
		} else if (!(strcmp(inputArgs[0], "jobs"))) {
			printProcesses(pList);
		} else {
			//If none of the built in functions, search the PATH for an executable
			Process * executed = (Process *) malloc(sizeof(Process));
			execute(inputArgs, executed);
			if (!nowait) {
				//If nowait flag is not set, wait until child process completes
				waitpid(executed->pid, &status, 0);
				printstatus(status, executed->pid, executed->procname);
			} else {
				//Put the process in the processList table.
				addToList(pList, executed);
				//Or, if nowait, then process in the background
				printf("Process %s executing in background\n",
						executed->procname);
			}
		}

		//Route stdout to stdout
		//(In case previous command was piped to a file)
		//close previous file descriptor
		close(fd);
		//Flush standard out
		fflush(stdout);
		//Redirect file stream to old standard out
		if (dup2(stdout_old, 1) < 0) {
			exit(-1);
		}
		//Close the saved old standard out file descriptor
		close(stdout_old);

	}
}
