#ifndef UTILS_H_
#define UTILS_H_
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


void sort(int * array, int arrSize);
int writeToFile(char * filename, char * content);

#endif /* UTILS_H_ */
