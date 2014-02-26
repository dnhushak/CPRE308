
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	extern char *optarg;
	extern int optind, opterr;
	int ch;
	char prompt[20] = "308sh> ";
	//Scans for argument inputs
	while ((ch = getopt(argc, argv, "p:")) != EOF) {
		switch (ch) {
		case 'p':
			strcpy(prompt,optarg);
			break;
		}

	}
	while(1){
		//User input prompt variables
		char input[30];
		char input2[30];
		
		//Display the shell prompt
		printf("%s",prompt);
		
		//Actually get the input
		fgets(input, 30, stdin);
		
		//Remove the newline from the end
		strncpy(input2, input, strlen(input)-1);
		
		//All of the checks for user input
		if(!(strcmp(input2,"ls"))){ 
			execute("/bin/ls", "ls");
		}
		else if(!(strcmp(input2,"exit"))){ 
			return 0;
		}		
		else if(!(strcmp(input2,"pid"))){ 
			printf("[%d]\n",getpid());
		}
		else
		{
			execute(input2, "");
		}
		
		//Resets the input string
		memset(input2, '\0', sizeof(input2) );
	}
}

int execute(char *fcn, char * const *args){
	int pid = fork();
	if (pid == 0){
		printf("Process %s executing with process ID: %d\n", args, getpid());
		execvp(fcn, args);
		return 0;
	}
	else{
		waitpid(pid);
	}
}