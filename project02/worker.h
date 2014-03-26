#ifndef WORKER_H_
#define WORKER_H_


#include <stdlib.h>
#include "commandList.h"
#include "utils.h"
#include "Bank.h"

typedef struct pthreadArgs {
	CommandList * cmdListArg;
	char * outFileArg;
	pthread_mutex_t * locksArg;
	int * numAccounts;
} pthreadArgs;

void * worker(void *);




#endif /* WORKER_H_ */
