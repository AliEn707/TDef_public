#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with players, get and proceed comands	                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

#define sendWorker(x,y) send(w->sock,x,y,MSG_NOSIGNAL)

static int checkEvent(void * n_n,event* e){
	worklist* w=n_n;
	player_info * pl=w->data;
	short l_l;
	char mes;
	if (pl->timestamp<e->timestamp){
		//do some stuff
		mes=MESSAGE_EVENT_CHANGE;
		sendData(w->sock,&mes,sizeof(mes));
		sendData(w->sock,&e->id,sizeof(e->id));
		sendWorker(&e->$rooms,sizeof(e->$rooms));
		l_l=strlen(e->map);
		sendWorker(&l_l,sizeof(l_l));
		sendWorker(e->map,l_l);
		
		//add another data
	}//TODO: how to send data about remove
	return 0;
}

int proceedPlayerMessage(worklist* w,char msg_type){
	player_info * pl=w->data;
//	int token;
	char msg;
/*	if (msg_type==MESSAGE_MOVE){
		recvData(w->sock,&msg,sizeof(msg));
		if (msg_type==MESSAGE_LOBBY){
			pl->status=PLAYER_IN_LOBBY;
			setMask(pl->bitmask,BM_PLAYER_STATUS);
		}
		if (msg_type==MESSAGE_MAP){
			pl->status=PLAYER_AT_MAP;
			setMask(pl->bitmask,BM_PLAYER_STATUS);
		}
		return 0;
	}
*/	if (msg_type==MESSAGE_ROOM_ACT){
		int room_type;
//		int room_token;
		int room_id;
		room * r_r;
//		token=rand(); 
		//get message type
		recvData(w->sock,&msg,sizeof(msg));
		printf("get mes %d\n",msg);
		//get room type
		recvData(w->sock,&room_type,sizeof(room_type));
		printf("get room type %d\n",room_type);
		//get all data
		
		//if player not in lobby dont do anithing
//		if (pl->status!=PLAYER_IN_LOBBY)
//				return 0;
		if (msg==MESSAGE_CREATE_ROOM){
			printf("ask to create room\n");
			//ask for room on server
			
			//-send ask, token
			//if ok create, set server data
			if ((r_r=malloc(sizeof(room)))==0){
				perror("malloc create room");
				return 0;
			}
			memset(r_r,0,sizeof(room));
			r_r->token=rand();//token;
			r_r->status=ROOM_CREATED;
			printf("token %d\n",r_r->token);
			t_semop(t_sem.room,&sem[0],1);		
			room_id=roomAdd(room_type,r_r);
			t_semop(t_sem.room,&sem[1],1);
			//pl->timestamp=time(0);
			if (pl->room.id!=0)
				roomLeave(pl->room.type,pl->room.id);
			
			pl->room.type=room_type;
			r_r->type=room_type;
			pl->room.id=room_id;
			r_r->id=room_id;
			r_r->players.max=2;//change to event players max
			roomEnter(pl->room.type,pl->room.id);
			
			pl->room.token=rand();
			//set to prepare/ server worker then ask for room,
			//when room will create, room server conn and set ROOM_RUN
			r_r->status=ROOM_PREPARE;
//			setMask(pl->bitmask,BM_PLAYER_TIMESTAMP);
			return 0;
		}
		if (msg==MESSAGE_FAST_ROOM){
			printf("ask to fast find room\n");
			//find room
			t_semop(t_sem.room,&sem[0],1);
			room_id=roomFind(room_type);
			t_semop(t_sem.room,&sem[1],1);
			//check we find room
			if (room_id==0){
				//send no rooms found
				printf("can't find room\n");
				return 0;
			}
			t_semop(t_sem.room,&sem[0],1);
			r_r=roomGet(room_type,room_id);
			t_semop(t_sem.room,&sem[1],1);
			if (r_r==0){
				//send no rooms found
				return 0;
			}
			
//			pl->timestamp=time(0);
			if (pl->room.id!=0)
				roomLeave(pl->room.type,pl->room.id);
			
			pl->room.type=room_type;
			pl->room.id=room_id;
			
			roomEnter(pl->room.type,pl->room.id);
			pl->room.token=rand();
			r_r->timestamp=time(0);
//			pl->status=PLAYER_IN_GAME;
//			setMask(pl->bitmask,BM_PLAYER_STATUS);
			//send conect to player
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

static int checkPlayerData(worklist* w,int _timestamp){
	player_info * pl=w->data;
	room * r_r;
	char mes;
//	if (pl->status==PLAYER_IN_LOBBY){
//		printf("player in lobby\n");
		//check player data
	if (pl->room.id!=0){
		t_semop(t_sem.room,&sem[0],1);
		r_r=roomGet(pl->room.type,pl->room.id);
		t_semop(t_sem.room,&sem[1],1);
//		printf("check room %p \n",room);
		if (r_r!=0){
//			printf("check room ts %d %d\n",pl->timestamp,r_r->timestamp);
			if (pl->timestamp<=r_r->timestamp)
				if (r_r->status==ROOM_RUN){
//					event * e_e;
//					pl->timestamp=room->timestamp;
//					pl->status=PLAYER_IN_GAME;
//					setMask(pl->bitmask,BM_PLAYER_STATUS);
					if (pl->room.timestamp!=r_r->timestamp){
						//send player to connect
						mes=MESSAGE_GAME_START;
						//send mes game start
						sendData(w->sock,&mes,sizeof(mes));
						char * s_s=serverGetById(r_r->server);
						short l_l=strlen(s_s);
						//send size of hostname
						sendData(w->sock,&l_l,sizeof(l_l));
						//send hostname
						sendData(w->sock,s_s,l_l);
						//send port
						sendData(w->sock,&r_r->port,sizeof(r_r->port));
						sendData(w->sock,&r_r->type,sizeof(r_r->type));
						pl->room.timestamp=r_r->timestamp;
					}
				}
		}else{
			//set player not chose room
			pl->room.id=0;
			//set player status not in lobby
		}
	}
		//
//	}
	//check events
	eventForEach(w, checkEvent);
	//other places
	pl->timestamp=_timestamp;
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
//		setMask(pl->bitmask,BM_PLAYER_STATUS);
	}
//	printf("send bitmask %d",pl->bitmask);
	//send bitmask
	sendData(w->sock,&pl->bitmask,sizeof(pl->bitmask));
	pl->bitmask=0;
	return 0;
}

void * threadWorker(void * arg){
	int id=*(int*)arg;
	worklist * tmp;
	worklist * task=&config.worker[id].client;
	t_sem_t worker_sem=t_sem.worker[id];
	int TPS=10;  //ticks per sec
	struct timeval tv={0,0};
	timePassed(&tv);
	free(arg);
	printf("Worker %d started\n",id);
	short err;
	while(config.run){
		int _timestamp=time(0);
		tmp=task;
		t_semop(worker_sem,&sem[0],1);
			for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
				//some work
				err=0;
				//get data from player if any
				if (recvPlayerData(tmp)!=0){
//					printf("cant recv\n");
					err++;
				}
				//check changes of player
				checkPlayerData(tmp,_timestamp);
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
		t_semop(worker_sem,&sem[1],1);
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
	
	if((t_sem.worker[id]=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	t_semop(t_sem.worker[id],&sem[1],1);
	
	if(pthread_create(&th,0,threadWorker,arg)!=0)
		return 0;
	return th;
}
