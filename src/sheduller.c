#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with held tasks					                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/



void * threadSheduller(void * arg){
	int id=*(int*)arg;
	int TPS=1;  //ticks per sec
	struct timeval tv={0,0};
	
	void* proceed(worklist* tmp, void* arg){
		//some work
		//send message
		return 0;
	}
		
	timePassed(&tv);
	free(arg);
	printf("Sheduller %d started\n",id);
	while(config.run){
		//check tasks
		t_semop(t_sem.sheduller,&sem[0],1);
			//do actions
			worklistForEachRemove(&config.sheduller.task,proceed,0);
		t_semop(t_sem.sheduller,&sem[1],1);
		
//		while(getMsg(msg)>0){
//			printf("get message %s\n",msg.buf);
			//some work
//		}
//		printf("ok\n");
		syncTPS(timePassed(&tv),TPS);
	}
	printf("close Sheduller\n");
	return 0;
}

pthread_t startSheduller(int id){
	pthread_t th=0;
	int * arg;
	struct sembuf _sem={0,WORKER_NUM,0};
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startSheduller");
	*arg=id;
	
	if((t_sem.sheduller=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	t_semop(t_sem.sheduller,&sem[1],1);
	
	if((t_sem.events=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	t_semop(t_sem.sheduller,&_sem,1);
	
	if(pthread_create(&th,0,threadSheduller,arg)!=0)
		return 0;
	return th;
}

