#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with players, get and proceed comands	                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


int proceedPlayerMessage(worklist* w,char msg_type){
	player_info * pl=w->data;
//	int token;
	char msg;
	if (msg_type==MESSAGE_MOVE){
		recvData(w->sock,&msg,sizeof(msg));
		if (msg_type==MESSAGE_LOBBY){
			pl->status=PLAYER_IN_LOBBY;
			setMask(pl->bitmask,BM_PLAYER_STATUS);
		}
		return 0;
	}
	if (msg_type==MESSAGE_LOBBY){
		int room_type;
//		int room_token;
		int room_id;
		room * room;
//		token=rand(); 
		//get message type
		recvData(w->sock,&msg,sizeof(msg));
		printf("get mes %d\n",msg);
		//get room type
		recvData(w->sock,&room_type,sizeof(room_type));
		printf("get room type %d\n",room_type);
		//get all data
		
		//if player not in lobby dont do anithing
		if (pl->status!=PLAYER_IN_LOBBY)
				return 0;
		if (msg==MESSAGE_CREATE_ROOM){
			printf("ask to create room\n");
			//ask for room on server
			
			//-send ask, token
			//if ok create, set server data
			room=malloc(sizeof(room));
			room->token=rand();//token;
			room_id=roomAdd(room_type,room);
			
			pl->timestamp=time(0);
			if (pl->room.id!=0)
				roomLeave(pl->room.type,pl->room.id);
			
			pl->room.type=room_type;
			pl->room.id=room_id;
			roomEnter(pl->room.type,pl->room.id);
			
			pl->room.token=rand();
			//set to prepare/ server worker then ask for room,
			//when room will create, room server conn and set ROOM_RUN
			room->status=ROOM_PREPARE;
//			setMask(pl->bitmask,BM_PLAYER_TIMESTAMP);
			return 0;
		}
		if (msg==MESSAGE_FAST_ROOM){
			//find room
			room_id=roomFind(room_type);
			//check we find room
			if (room_id==0){
				//send no rooms found
				return 0;
			}
			pl->timestamp=time(0);
			if (pl->room.id!=0)
				roomLeave(pl->room.type,pl->room.id);
			
			pl->room.type=room_type;
			pl->room.id=room_id;
			
			roomEnter(pl->room.type,pl->room.id);
			pl->room.token=rand();
			pl->status=PLAYER_IN_GAME;
			setMask(pl->bitmask,BM_PLAYER_STATUS);
			//set token
			return 0;
		}
		if (msg==MESSAGE_FIND_ROOM){ //not used
			//find rooms
			
			//add something
			
			//set token
			return 0;
		}
	}	
	
	return 1;
}


int recvPlayerData(worklist* w){
	int i;
	char msg_type;
//	printf("try to get message\n");
	//try to read message from client 10 times
	for(i=0;i<10;i++){
		if (recv(w->sock,&msg_type,sizeof(msg_type),MSG_DONTWAIT)<0){
			//have error check what is it
			if (errno==EAGAIN){
//				sleep(0);
//				continue;
				break;
			}else{
				perror("recv threadWorker");
				return 1;
			}			
		}else{
			printf("got message %d\n",msg_type);
			//get message need to proseed
			if (proceedPlayerMessage(w,msg_type)!=0)
				return 1;
		}
	}
	
	return 0;
}

int checkPlayerData(worklist* w){
	player_info * pl=w->data;
	room * room;
	if (pl->status==PLAYER_IN_LOBBY){
//		printf("player in lobby\n");
		//check player data
		if (pl->room.id!=0){
			room=roomGet(pl->room.type,pl->room.id);
			printf("get get room %d \n",(int)room);
			if (room!=0){
				if (pl->timestamp<room->timestamp)
					if (room->status==ROOM_RUN){
						pl->timestamp=room->timestamp;
						pl->status=PLAYER_IN_GAME;
						setMask(pl->bitmask,BM_PLAYER_STATUS);
					}
			}else{
				//set player not chose room
				pl->room.id=0;
				//set player status not in lobby
			}
		}
		//
	}
	//other places
	return 0;
}

int sendPlayerData(worklist* w){
//	char msg_type;
	player_info * pl=w->data;
	char mes;
	if (pl->bitmask==0)
		return 0;
	mes=MESSAGE_PLAYER_CHANGE;
	sendData(w->sock,&mes,sizeof(mes));
//	printf("send mes %d",mes);
	//correct bitmask
	if (checkMask(pl->bitmask,BM_PLAYER_CONNECTED)){
		//send data to client
		setMask(pl->bitmask,BM_PLAYER_STATUS);
	}
//	printf("send bitmask %d",pl->bitmask);
	//send bitmask
	sendData(w->sock,&pl->bitmask,sizeof(pl->bitmask));
	if (checkMask(pl->bitmask,BM_PLAYER_STATUS)){
		//send data to client
		sendData(w->sock,&pl->status,sizeof(pl->status));
	}
	pl->bitmask=0;
	return 0;
}

void * threadWorker(void * arg){
	int id=*(int*)arg;
	worklist * tmp;
	worklist * task=&config.worker[id].client;
	int worker_sem=config.worker[id].sem;
	int TPS=10;  //ticks per sec
	struct timeval tv={0,0};
	timePassed(&tv);
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
//					printf("cant recv\n");
					err++;
				}
				//check changes of player
				checkPlayerData(tmp);
				//send data to player if need
				if (sendPlayerData(tmp)!=0){
//					printf("cant send\n");
					err++;
				}
				if (err!=0){
//					printf("lost connection\n");
					//set player lost connection
					((player_info*)(tmp->data))->conn=FAIL;
					tmp=worklistDel(task,tmp->id);
				}
			}
		semop(worker_sem,&sem[1],1);
		//some work
		syncTPS(timePassed(&tv),TPS);
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
