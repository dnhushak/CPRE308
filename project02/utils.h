#ifndef UTILS_H_
#define UTILS_H_
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int checkDuplicates(int * array, int arrSize);
void parallelSort(int *, int *, int arrSize);
int writeToFile(char * filename, char * content);

#endif /* UTILS_H_ */
