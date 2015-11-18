#include <stdarg.h>

#include "headers.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 		main engine functions		 			                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


int _sendData(int sock, void * buf, int size){
	int need=size;
	int get;
	get=send(sock,buf,need,MSG_NOSIGNAL);
	if (get<=0)
		return get;
	if (get==need)
		return get;
	printf("send not all\n");
	while(need>0){
		need-=get;
		if((get=send(sock,buf+(size-need),need,MSG_NOSIGNAL))<=0)
			return get;
	}
	return size;
}


int recvData(int sock, void * buf, int size){
	int need=size;
	int get;
	get=recv(sock,buf,need,MSG_DONTWAIT);
	if (get==0)
		return 0;
	if (get<0)
		if (errno!=EAGAIN)
			return -1;
	if (get==need)
		return get;
//	printf("get not all\n");
	int $$=0;
	do{
//		printf("try to read %d\n",$$);
		if (get>0)
			need-=get;
//		printf("try to get\n");
		if((get=recv(sock,buf+(size-need),need,MSG_DONTWAIT))<=0)
			if (errno!=EAGAIN)
				return -1;
		usleep(100000);
		$$++;
		if ($$>20)//max tries of read
			return -1;
	}while(need>0);
	return size;
}

void printLog(const char* format, ...) {
	if (config.debug == 0)
		return;
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stdout, format, argptr);
	va_end(argptr);	
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

//check and free player slots
static inline int newPlayerId(){
/*	int i;
	for(i=1;i<PLAYER_MAX;i++)
		if (config.player.ids[i]==0){
			config.player.ids[i]++;
			return i;
		}
*/
	if (config.$players<PLAYER_MAX){
		config.$players++;
		return 1;
	}
	printf("not enough player slots\n");
	return -1;
}

//free player slot
static inline int delPlayerId(){
	
//	config.player.ids[id]=0;
	config.$players--;
	return -1;
}

player_info * newPlayer(){
	player_info * pl;
	if (newPlayerId()<0)
		return 0;
	do {
		if ((pl=malloc(sizeof(*pl)))==0){
			perror("malloc newPlayer");
			return 0;
		}
		memset(pl,0,sizeof(*pl));
		if((pl->sem=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0) //do not take access to player info from another
			break;
		t_semop(pl->sem,&sem[1],1);
		
		return pl;
	}while(0);
	free(pl);
	return 0;
}

//cleanup player_info
void realizePlayer(void *v){
	player_info * pl=v;
	delPlayerId();
	if (t_semctl(pl->sem,0,IPC_RMID)<0)
			perror("t_semctl player");
	free(pl);
}

void cleanAll(){
	int i;
	for(i=0;i<WORKER_NUM;i++){
		if (t_sem.worker[i]!=0)	
			if (t_semctl(t_sem.worker[i],0,IPC_RMID)<0)
				perror("t_semctl worker");
		worklistErase(&config.worker[i].client);
	}
	
	if (t_sem.db!=0)	
		if (t_semctl(t_sem.db,0,IPC_RMID)<0)
			perror("t_semctl db");
		
	if (t_sem.serverworker!=0)	
		if (t_semctl(t_sem.serverworker,0,IPC_RMID)<0)
			perror("t_semctl serverworker");
	if (t_sem.room!=0)	
		if (t_semctl(t_sem.room,0,IPC_RMID)<0)
			perror("t_semctl room serverworker");
	worklistErase(&config.serverworker.client);
	
	if (t_sem.watcher!=0)	
		if (t_semctl(t_sem.watcher,0,IPC_RMID)<0)
			perror("t_semctl watcher");
	worklistErase(&config.watcher.client);
		
	if (t_sem.sheduller!=0)	
		if (t_semctl(t_sem.sheduller,0,IPC_RMID)<0)
			perror("t_semctl sheduller");
	if (t_sem.events!=0)	
		if (t_semctl(t_sem.events,0,IPC_RMID)<0)
			perror("t_semctl events");
	worklistErase(&config.sheduller.task);
	
	if (t_sem.updater!=0)	
		if (t_semctl(t_sem.updater,0,IPC_RMID)<0)
			perror("t_semctl sheduller");
	worklistErase(&config.updater.task);
	
	if (t_sem.player!=0)	
		if (t_semctl(t_sem.player,0,IPC_RMID)<0)
			perror("t_semctl player");
	bintreeErase(&config.player.tree, realizePlayer);
		
	serversClean();
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