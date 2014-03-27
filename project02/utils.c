#include <stdio.h>
#include "utils.h"

void parallelSort(int * arrayToSortBy, int * arrayToSortWith, int arrSize) {
	int c, d;

	for (c = 0; c < (arrSize - 1); c++) {
		for (d = 0; d < arrSize - c - 1; d++) {
			if (arrayToSortBy[d] > arrayToSortBy[d + 1]) {
				//Swap the key array
				arrayToSortBy[d] ^= arrayToSortBy[d + 1];
				arrayToSortBy[d + 1] ^= arrayToSortBy[d];
				arrayToSortBy[d] ^= arrayToSortBy[d + 1];

				// Swap the parallel array
				arrayToSortWith[d] ^= arrayToSortWith[d + 1];
				arrayToSortWith[d + 1] ^= arrayToSortWith[d];
				arrayToSortWith[d] ^= arrayToSortWith[d + 1];
			}
		}
	}
}

int checkDuplicates(int * array, int arrSize) {
	int i, j;
	for (i = 0; i< arrSize; i++){
		for (j = 0; j<i; j++){
			if(array[i] == array[j]){
				// Found a dupe
				return 1;
			}
		}
	}
	// No duplicates
	return 0;
}

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
