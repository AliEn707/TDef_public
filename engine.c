#include "headers.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 		main engine functions		 			                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


int recvData(int sock, void * buf, int size){
	int need=size;
	int get;
	get=recv(sock,buf,need,0);
	if (get<0)
		return -1;
	if (get==need)
		return get;
//	printf("get not all\n");
	while(need>0){
		need-=get;
		if((get=recv(sock,buf+(size-need),need,0))<0)
			return 0;
	}
	return size;
}



//time passed after previous call of function
int timePassed(struct timeval * t){
	//config.time  struct timeval
	struct timeval end;
	gettimeofday(&end, NULL);
	int out=((end.tv_sec - t->tv_sec)*1000000+
			end.tv_usec - t->tv_usec);
	memcpy(t,&end,sizeof(end));
	return out;
}

void syncTPS(int z,int TPS){
	if((z=(1000000/TPS)-z)>0){
		usleep(z);
	}
}

int newPlayerId(){
	int i;
	for(i=1;i<PLAYER_MAX;i++)
		if (config.player.ids[i]==0)
			return i;
	return -1;
}

int delPlayerId(int id){
	config.player.ids[id]=0;
	return -1;
}

void cleanAll(){
	int i;
	for(i=0;i<WORKER_NUM;i++){
		if (config.worker[i].sem!=0)	
			if (semctl(config.worker[i].sem,0,IPC_RMID)<0)
				perror("semctl worker");
		worklistErase(&config.worker[i].client);
	}
	
	if (config.serverworker.sem!=0)	
		if (semctl(config.serverworker.sem,0,IPC_RMID)<0)
			perror("semctl serverworker");
	worklistErase(&config.serverworker.client);
	
	if (config.watcher.sem!=0)	
		if (semctl(config.watcher.sem,0,IPC_RMID)<0)
			perror("semctl watcher");
	worklistErase(&config.watcher.client);
		
	if (config.sheduller.sem!=0)	
		if (semctl(config.sheduller.sem,0,IPC_RMID)<0)
			perror("semctl sheduller");
	worklistErase(&config.sheduller.task);
	
	//clear msg
	if (config.sheduller.msg!=0)	
		if (msgctl(config.sheduller.msg,IPC_RMID,0))
			perror("mesctl sheduller");
	
	if (config.player.sem!=0)	
		if (semctl(config.player.sem,0,IPC_RMID)<0)
			perror("semctl player");
	bintreeErase(&config.player.tree);
}

/*
void syncTime(struct timeval *t, unsigned int time){
	struct timeval end;
	gettimeofday(&end, NULL);
	int out=((end.tv_sec - *t.tv_sec)*1000+
			(end.tv_usec - *t.tv_usec)/1000.0);
	if (out<time)
		usleep((time-out)*1000);
	*t.tv_sec=end.tv_sec;
	*t.tv_usec=end.tv_usec;
}
*/