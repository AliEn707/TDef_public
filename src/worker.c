#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with players, get and proceed comands	                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

#define sendWorker(x,y) send(w->sock,x,y,MSG_NOSIGNAL)


static int proceedPlayerMessage(worklist* w,char msg_type){
	player_info * pl=w->data;
//	int token;
	char msg;
	if (msg_type==MESSAGE_ROOM_ACT){
		int room_type;
//		int room_token;
		int room_id;
		room * r_r;
//		token=rand(); 
		//get message type
		recvData(w->sock,&msg,sizeof(msg));
		printf("got mes %d\n",msg);
		//get room type
		recvData(w->sock,&room_type,sizeof(room_type));
		printf("got room type %d\n",room_type);
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
			//pl->timestamp=time(0);
			if (pl->room.id!=0){
				roomLeave(pl->room.type,pl->room.id);
			}
			t_semop(t_sem.room,&sem[1],1);

			pl->room.type=room_type;
			r_r->type=room_type;
			r_r->players.max=2;//change to event players max

			t_semop(t_sem.room,&sem[0],1);
			room_id=roomAdd(room_type,r_r);
			r_r->id=room_id;
			pl->room.id=room_id;
			roomEnter(pl->room.type,pl->room.id);
			r_r->status=ROOM_PREPARE;
			t_semop(t_sem.room,&sem[1],1);
			
			pl->room.token=rand();
			setMask(pl->bitmask,BM_PLAYER_ROOM);
			//set to prepare/ server worker then ask for room,
			//when room will create, room server conn and set ROOM_RUN
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
			if (r_r!=0)
				r_r->timestamp=time(0);
			t_semop(t_sem.room,&sem[1],1);
			
//			pl->timestamp=time(0);
			if (pl->room.id!=0){
				t_semop(t_sem.room,&sem[0],1);
				roomLeave(pl->room.type,pl->room.id);
				t_semop(t_sem.room,&sem[1],1);
			}
			setMask(pl->bitmask,BM_PLAYER_ROOM);
			if (r_r==0){
				pl->room.id=0;
				//send no rooms found
				return 0;
			}
			
			pl->room.type=room_type;
			pl->room.id=room_id;
			
			t_semop(t_sem.room,&sem[0],1);
			roomEnter(pl->room.type,pl->room.id);
			t_semop(t_sem.room,&sem[1],1);
			
			pl->room.token=rand();
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
	if (msg_type==MESSAGE_INFO){
		recvData(w->sock,&msg,sizeof(msg));
		if (msg==MESSAGE_EVENT_INFO){ //not used
			
		}
	}
	printf("unknown message\n");
	return 1;
}


static inline int recvPlayerData(worklist* w){
	int i;
	char msg_type;
//	printf("try to get message\n");
	//try to read message from client 10 times
	for(i=0;i<10;i++){
		if (recv(w->sock,&msg_type,sizeof(msg_type),MSG_DONTWAIT)<0){
			//have error check what is it
			if (errno==EAGAIN){
				errno=0;
				usleep(100);
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

static inline int checkPlayerEvents(worklist * w,time_t _timestamp){
	player_info * pl=w->data;
	int sendEventChanged(event* e){
		short l_l;
		char mes;
		if (e==0)
			return 0;
		mes=MESSAGE_EVENT_CHANGE;
		sendData(w->sock,&mes,sizeof(mes));
//		sendData(w->sock,&bitmask,sizeof(bitmask));
		sendData(w->sock,&e->id,sizeof(e->id));
//		sendWorker(&e->$rooms,sizeof(e->$rooms));
		l_l=strlen(e->map);
		sendData(w->sock,&l_l,sizeof(l_l));
		sendData(w->sock,e->map,l_l);
		l_l=strlen(e->name);
		sendData(w->sock,&l_l,sizeof(l_l));
		sendData(w->sock,e->name,l_l);
		//
		bintreeDel(&pl->events.available, e->id, 0);
		bintreeAdd(&pl->events.sent, e->id, (void*)(long)e->id);
		return 0;
	}
	int sendEventDroped(event* e){
		if (e==0)
			return 0;
		//TODO: add sent info
		bintreeDel(&pl->events.droped, e->id, 0);
		return 0;
	}
	void checkEventAvailable(int k, void*v, void* arg){
		sendEventChanged(eventGet((long)v));
	}
	void checkEventDroped(int k, void*v, void* arg){
		sendEventDroped(eventGet((long)v));
	}
	bintree* sent;
	int checkEvent(void * n_n,event* e){
		worklist* w=n_n;
		player_info * pl=w->data;
		bintreeDel(sent,e->id,0);
		//TODO: add check for dependences
		//if need to send number of rooms need to add check(||) for timestamp
		if (bintreeGet(&pl->events.sent, e->id)==0){
			if (bintreeGet(&pl->events.done, e->id)==0)
				bintreeAdd(&pl->events.available, e->id, (void*)(long)e->id);
			//add another checks
		}//TODO: how to send data about remove
		return 0;
	}
	/*
		events can be added: 
			after events update
			after map done
	*/
	//check for new events
	if (!pl->events.updated ||
			config.events.timestamp>pl->events.timestamp ||
			checkMask(pl->bitmask,BM_PLAYER_CONNECTED)){
		//create copy of sent events
		sent=bintreeClone(&pl->events.sent);
		//add events to available;
		eventForEach(w, checkEvent);
		//drop remained events
		void checkDrop(int k,void*v,void* arg){
			bintreeAdd(arg,k,v);
		}
		bintreeForEach(sent,checkDrop,&pl->events.droped);
		//clean end free bintree 
		bintreeErase(sent,0);
		free(sent);
		pl->events.timestamp=_timestamp;
		pl->events.updated=1;
	}
	//send info about available events
	bintreeForEach(&pl->events.available,checkEventAvailable,0);
	//send info about dropped events
	bintreeForEach(&pl->events.droped,checkEventDroped,0);
	
	return 0;
}

static int checkPlayerRoom(worklist * w, time_t _timestamp){
	player_info * pl=w->data;
	room * r_r;
	char mes;
//	printf("player %d room_id %d\n",pl->id,pl->room.id);
		//check room data
	if (pl->room.id!=0){
		t_semop(t_sem.room,&sem[0],1);
		r_r=roomGet(pl->room.type,pl->room.id);
		t_semop(t_sem.room,&sem[1],1);
//		printf("check room %p \n",r_r);
		if (r_r!=0){
//			printf("check room ts %d %d\n",pl->timestamp,r_r->timestamp);
			if (pl->timestamp<=r_r->timestamp){
				if (r_r->status==ROOM_RUN){
//					printf("check room timestamp\n");
					if (pl->room.timestamp!=r_r->timestamp){//TODO: check why 
						printf("send room ready\n");
						//send player to connect
						mes=MESSAGE_GAME_START;
						//send mes game start
						sendData(w->sock,&mes,sizeof(mes));
						sendData(w->sock,&r_r->type,sizeof(r_r->type));
						char * s_s=serverGetById(r_r->server);
						short l_l=strlen(s_s);
						//send size of hostname
						sendData(w->sock,&l_l,sizeof(l_l));
						//send hostname
						sendData(w->sock,s_s,l_l);
						//send port
						sendData(w->sock,&r_r->port,sizeof(r_r->port));
						pl->room.timestamp=r_r->timestamp;
					}
				}
				if (r_r->status==ROOM_FAIL){
					//need to leave room
					printf("room fail, leave room\n");
					t_semop(t_sem.room,&sem[0],1);
					roomLeave(pl->room.type,pl->room.id);
					t_semop(t_sem.room,&sem[1],1);
					pl->room.id=0;
					setMask(pl->bitmask,BM_PLAYER_ROOM);
				}
			}
		}else{
			//set player not chose room
			pl->room.id=0;
			setMask(pl->bitmask,BM_PLAYER_ROOM);
		}
	}
	return 0;
}

static inline int checkPlayerData(worklist* w, time_t _timestamp){
	player_info * pl=w->data;
	checkPlayerRoom(w,_timestamp);
	checkPlayerEvents(w,_timestamp);
	//other places
	pl->timestamp=_timestamp;
	return 0;
}

static inline int sendBeep(worklist* w){
	char mes=0;
	sendData(w->sock,&mes,sizeof(mes));
	return 0;
}

static inline int sendPlayerData(worklist* w){
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
	//send bitmask
	sendData(w->sock,&pl->bitmask,sizeof(pl->bitmask));
	printf("sent bitmask %d\n",pl->bitmask);
	sendData(w->sock,&pl->id,sizeof(pl->id));
	printf("sent pl->id %d\n",pl->id);
	if (checkMask(pl->bitmask,BM_PLAYER_ROOM)){
		//send data to client
		sendData(w->sock,&pl->room.type,sizeof(pl->room.type));
		printf("sent pl->room.type %d\n",pl->room.type);
		sendData(w->sock,&pl->room.id,sizeof(pl->room.id));
		printf("sent pl->room.id %d\n",pl->room.id);
//		setMask(pl->bitmask,BM_PLAYER_STATUS);
	}
//	printf("send bitmask %d",pl->bitmask);
	pl->bitmask=0;
	
	return 0;
}

void * threadWorker(void * arg){
	int id=*(int*)arg;
	int TPS=10;  //ticks per sec
	struct timeval tv={0,0};
	time_t _timestamp;
	long err;
	
	void* proceed(worklist* tmp, void* arg){
		//some work
		err=0;
		//get data from player if any
		if (recvPlayerData(tmp)!=0){
//		printf("cant recv\n");
			err++;
		}
		//check changes of player
		checkPlayerData(tmp,_timestamp);
		sendBeep(tmp);
		//send data to player if need
		if (sendPlayerData(tmp)!=0){
//		printf("cant send\n");
			err++;
		}
		if (err!=0){
			printf("player %d lost connection\n", ((player_info*)(tmp->data))->id);
			//set player lost connection
			((player_info*)(tmp->data))->conn=FAIL;
			config.worker[id].client_num--;
		}
		//if err!=0, tmp will be removed
		return (void*)err;
	}
	
	timePassed(&tv);
	free(arg);
	printf("Worker %d started\n",id);
	while(config.run){
		_timestamp=time(0);
		t_semop(t_sem.worker[id],&sem[0],1);
			//do actions
			worklistForEachRemove(&config.worker[id].client,proceed,0);
		t_semop(t_sem.worker[id],&sem[1],1);
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
