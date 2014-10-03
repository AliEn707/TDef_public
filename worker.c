#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with players, get and proceed comands	                       ║
╚══════════════════════════════════════════════════════════════╝
*/


void * threadWorker(void * arg){
	int id=*(int*)arg;
	worklist * tmp;
	worklist * task=&config.worker[id].client;
	int worker_sem=config.worker[id].sem;
	free(arg);
	printf("start Worker %d\n",id);
	
	while(config.run){
		tmp=task;
		semop(worker_sem,&sem[0],1);
			for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
				//some work
			}
		semop(worker_sem,&sem[1],1);
		//some work
		usleep(100);
	}
	printf("close Worker\n");
	return 0;
}

pthread_t  startWorker(int id){
	pthread_t th=0;
	int * arg;
	
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startWorker");
	*arg=id;
	
	if((config.worker[id].sem=semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	semop(config.worker[id].sem,&sem[2],1);
	
	if(pthread_create(&th,0,threadWorker,arg)!=0)
		return 0;
	return th;
}
