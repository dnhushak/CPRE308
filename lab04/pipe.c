#include <stdio.h>
#define MSGSIZE 16
int main() {
	// Declaring and initializing the string
	char *msg = "How are you?";

	// Declaring an input buffer for a pipe to write to
	char inbuff[MSGSIZE];

	// Sets up the pipe file descriptors
	// There are only two, and p[0] is the read end of the pipe,
	// and p[1] is the write end of the pipe
	int p[2];

	// PID holding variable
	int ret;

	// Make the pipe
	pipe(p);

	// Fork the program
	ret = fork();
	if (ret > 0) {

		// If the parent process, write msg to the pipe
		write(p[1], msg, MSGSIZE);
	} else {
		// Ensure that the parent program writes to the pipe before the child reads from it
		sleep(1);

		// If the child process, read from the pipe into the input buffer
		read(p[0], inbuff, MSGSIZE);

		// Print the input buffer
		printf("%s\n", inbuff);
	}
	exit(0);
}
