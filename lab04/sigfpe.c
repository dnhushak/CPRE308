#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
void fpe_routine();

int main(int argc, char * argv[]) {
	// Numerator, Denominator, and Output values
	int numer = 1;
	int denom = 1;
	int out;

	// If an argument is given, set the numerator to it
	if (argc > 1) {
		// Convert string to int
		numer = atoi(argv[1]);
	}

	// If a second argument is given, set the denominator to it
	if (argc > 2){
		// Convert string to int
		denom = atoi(argv[2]);
	}

	// Set up the signal handler
	signal(SIGFPE, fpe_routine);

	// Perform the division
	out = numer / denom;

	//Output the result
	printf("Output: %d\n", out);
	return out;

}

void fpe_routine() {
	// Only enters on a SIGFPE signal, print a warning and exit
	printf("caught a SIGFPE\n");
	exit(0);
}
