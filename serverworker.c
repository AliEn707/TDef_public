#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread wark with play rooms get statistics and other data                        ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


void * threadServerWorker(void * arg){
	int id=*(int*)arg;
	worklist * tmp;
	free(arg);
	printf("ServerWorker %d started\n",id);
	
	while(config.run){
		tmp=&config.serverworker.client;
		semop(config.serverworker.sem,&sem[0],1);
			for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
				//get data from server about players and statistics
				
				//close socket
				//delete client
			}
		semop(config.serverworker.sem,&sem[1],1);
		//some work
		usleep(100);
	}
	printf("close ServerWorker\n");
	return 0;
}

pthread_t  startServerWorker(int id){
	pthread_t th=0;
	int * arg;
	
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startServerWorker");
	*arg=id;
	
	if((config.serverworker.sem=semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	semop(config.serverworker.sem,&sem[1],1);
	
	if(pthread_create(&th,0,threadServerWorker,arg)!=0)
		return 0;
	return th;
}

