#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
char msg[100];
void my_alarm();

int main(int argc, char * argv[]) {
	int time;
	if (argc < 3) {
		// Checks for valid number of parameters
		printf("not enough parameters\n");
		exit(1);
	}

	// Converts the second argument from a string to an int
	// Sets time to second argument
	time = atoi(argv[2]);

	// Copies the first argument into the message buffer
	strcpy(msg, argv[1]);

	// Set up the signal handler for the SIGALARM signal
	signal(SIGALRM, my_alarm);

	// Tell the program to send a SIGALARM after time seconds
	alarm(time);

	// Enter the infinite loop
	printf("Entering infinite loop\n");

	int i;
	while (1) {
		// Print out a timer, indicating elapsed number of seconds
		printf("%d\n", i++);
		sleep(1);
	}

	// Can't get here
	printf("Can't get here\n");
}
void my_alarm() {
	// When the alarm is caught by the signal handler, enter here

	// Print the message stored in the message buffer to stdout
	printf("%s\n", msg);

	// Exit
	exit(0);
}
