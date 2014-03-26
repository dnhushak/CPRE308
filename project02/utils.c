#include <stdio.h>
#include "utils.h"

void sort(int * array, int arrSize)
{
  int c, d;

  for (c = 0 ; c < ( arrSize - 1 ); c++)
  {
    for (d = 0 ; d < arrSize - c - 1; d++)
    {
      if (array[d] > array[d+1])
      {
        array[d] ^= array[d+1];
        array[d+1] ^= array[d];
        array[d] ^= array[d+1];
      }
    }
  }
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
