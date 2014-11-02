#include "headers.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ thread listen sockets 							                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

/*
You can often see in this code $_$. 
$_$ is the Scrooge_McDuck. 
He helps us in this not simple work. 
*/
//Time.now.to_i in ruby to get time() val

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
	
	config.serverworker.port=7000;
	config.player.port=7001;
						  
	dbFillEvents();
	//set tasks for sheduller
	
	//start threads
//	config.sheduller.thread=startSheduller(0);
	config.serverworker.thread=startServerWorker(0);
	config.watcher.thread=startWatcher(0);
	for(i=0;i<WORKER_NUM;i++)
		config.worker[i].thread=startWorker(i);
	config.listener.thread=startListener();
	//start console for external commands
	
	//some garbage					  
//	for(i=0;i<20;i++){
//		sprintf(msg.buf,"hello");
//		sendMsg(msg);
//		usleep((rand()%9)*100000);
//	}
	printf("done\n"); 
	scanf("%s",msg.buf);
	printf("exiting\n");
	config.run=0;
	sleep(5);
	cleanAll();
	return 0;
}