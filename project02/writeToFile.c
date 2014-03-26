#include "writeToFile.h"

int writeToFile(char * filename, char * content) {
	int stdout_old = dup(1);
	int fd = 1;
	//Open the file
	fd = open(filename, O_WRONLY | O_APPEND | O_CREAT,
	S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

	//Redirect standard out
	if (dup2(fd, 1) < 0 || fd < 0) {
		return -1;
	}

	printf("%s", content);

	//Route stdout to stdout
	//close previous file descriptor
	close(fd);
	//Flush standard out
	fflush(stdout);
	//Redirect file stream to old standard out
	if (dup2(stdout_old, 1) < 0) {
		return -1;
	}
	//Close the saved old standard out file descriptor
	close(stdout_old);

	return 1;

}
