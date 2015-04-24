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
	worklist * tmp;
	int TPS=1;  //ticks per sec
	struct timeval tv={0,0};
	timePassed(&tv);
	free(arg);
	printf("Sheduller %d started\n",id);
	while(config.run){
		//check tasks
		tmp=&config.sheduller.task;
		t_semop(t_sem.sheduller,&sem[0],1);
			for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
				//some work
				//send message
				
			}
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
	
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startSheduller");
	*arg=id;
	
	if((t_sem.sheduller=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	t_semop(t_sem.sheduller,&sem[1],1);
	
	if(pthread_create(&th,0,threadSheduller,arg)!=0)
		return 0;
	return th;
}

