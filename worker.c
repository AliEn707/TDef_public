#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with players, get and proceed comands	                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


int proceedMessage(worklist* w,char msg_type){
	player_info * pl=w->data;
	int token;
	if (msg_type==MESSAGE_CREATE_ROOM){
//		printf("ask to create room\n");
		//get another data
		
		//if player not in lobby dont do anithing
		if (pl->status!=PLAYER_IN_LOBBY)
			return 0;
		token=rand(); 
		//ask for room on server
		//-send ask, token
		//if ok create, set server data
		room_info * room=roomNew();
		room->info->token=token;
		
		pl->timestamp=time(0);
		pl->room.id=room;
		pl->room.token=rand();
//		setMask(pl->bitmask,BM_PLAYER_TIMESTAMP);
		return 0;
	}
	if (msg_type==MESSAGE_FIND_ROOM){
		//get another data
		if (pl->status!=PLAYER_IN_LOBBY)
			return 0;
		token=rand(); 
		//find room
		
		//set token
		//send data
		return 0;
	}
	
	
	return 0;
}


int recvPlayerData(worklist* w){
	int i;
	char msg_type;
//	player_info * pl=w->data;
	//try to read message from client 10 times
	for(i=0;i<10;i++){
		if (recv(w->sock,&msg_type,sizeof(msg_type),MSG_DONTWAIT)<0){
			//have error check what is it
			if (errno==EAGAIN){
				sleep(0);
				continue;
			}else{
				perror("recv threadWorker");
				return 1;
			}
		}
		//get message need to proceed
		proceedMessage(w,msg_type);
	}
	
	return 0;
}

int checkPlayerData(worklist* w){
	player_info * pl=w->data;
	if (pl->status==PLAYER_IN_LOBBY){
//		printf("player in lobby\n");
		//check player data
		//set mask
	}
	//other places
	return 0;
}

int sendPlayerData(worklist* w){
	char msg_type;
	player_info * pl=w->data;
	if (checkMask(pl->bitmask,BM_PLAYER_CONNECTED)){
		//send data to client
		
	}
	pl->bitmask=0;
	return 0;
}

void * threadWorker(void * arg){
	int id=*(int*)arg;
	worklist * tmp;
	worklist * task=&config.worker[id].client;
	int worker_sem=config.worker[id].sem;
	free(arg);
	printf("Worker %d started\n",id);
	short err;
	while(config.run){
		tmp=task;
		semop(worker_sem,&sem[0],1);
			for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
				//some work
				err=0;
				//get data from player if any
				if (recvPlayerData(tmp)!=0){
					err++;
				}
				//check changes of plyer
				checkPlayerData(tmp);
				//send data to player if need
				if (sendPlayerData(tmp)!=0){
					err++;
				}
				if (err!=0){
					//set player lost connection
					//del from worklist
				}
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
	semop(config.worker[id].sem,&sem[1],1);
	
	if(pthread_create(&th,0,threadWorker,arg)!=0)
		return 0;
	return th;
}
