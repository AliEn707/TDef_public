#include "headers.h"


void * threadWorker(void * arg){
	int id=*arg;
	free(arg);
	printf("start Worker %d",id);
	while(config.run){
		//some work
	}
	printf("close Worker\n");
}

int startWorker(int id){
	pthread_t th=0;
	int * arg;
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startWorker");
	if(pthread_create(&th,0,threadWorker,arg)!=0)
		return 0;
	return th;
}

