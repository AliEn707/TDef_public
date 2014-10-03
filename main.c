#include "headers.h"



int main(){
	int i;
	message msg;
	struct sembuf _sem[2]={{0,-1,0},
						  {0,1,0}};
	//clean config
	memset(&config,0,sizeof(config));
	memcpy(&sem,&_sem,sizeof(sem));
	//set params
	srand(time(0));
	config.run=1;
	//set tasks for sheduller
	
	//start threads
	config.sheduller.thread=startSheduller(0);
	config.serverworker.thread=startServerWorker(0);
	for(i=0;i<WORKER_NUM;i++)
		config.worker[i].thread=startWorker(i);
	
	//start console for external commands
	
	//some garbage					  
	for(i=0;i<20;i++){
		sprintf(msg.buf,"hello");
		sendMsg(msg);
		usleep((rand()%9)*100000);
	}
	
	config.run=0;
	sleep(5);
	cleanAll();
	return 0;
}