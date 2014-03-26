#include "writeToFile.h"

int writeToFile(char * filename, char * content) {
	FILE * outFile;
	outFile = fopen(filename, "a");
	fprintf(outFile, "%s", content);
	fclose(outFile);

	return 1;

}
