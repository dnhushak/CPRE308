#include "writeToFile.h"

int writeToFile(char * filename, char * content) {
	// File pointer
	FILE * outFile;

	// Open the file in append style
	outFile = fopen(filename, "a");

	// Write to the file
	fprintf(outFile, "%s", content);

	// Close the file
	fclose(outFile);

	// Return 1 on success
	return 1;

}
