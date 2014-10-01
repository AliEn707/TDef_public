#include "headers.h"


void * threadSheduller(void * arg){
	int id=*arg;
	free(arg);
	printf("start Worker %d",id);
	while(config.run){
		//some work
	}
	printf("close Sheduller\n");
}

int startSheduller(int id){
	pthread_t th=0;
	int * arg;
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startSheduller");
	if(pthread_create(&th,0,threadSheduller,arg)!=0)
		return 0;
	return th;
}

