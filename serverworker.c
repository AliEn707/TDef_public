#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread wark with play rooms get statistics and other data                        ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/




static int connSendRecv(char * hostname,int port, void * mes, int $mes){
	int sockfd,n;
	struct sockaddr_in servaddr;
	struct hostent *server;
	char mes_;
	struct {char type;int token;} *message=mes;
	if (hostname==0)
		return -1;
	printf("try %s:%d\n",hostname,port);
	server = gethostbyname(hostname);
	if (server == NULL) {
		perror("gethostbyname");
		return -1;
	}
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		return -1;
	}
	
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	memcpy((char *)&servaddr.sin_addr.s_addr,(char *)server->h_addr, server->h_length);
//	servaddr.sin_addr.s_addr=inet_addr("172.16.1.40");//argv[1]);
	servaddr.sin_port=htons(port);

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
		perror("connect");
		return -2;
	}	
	
	sendData(sockfd,&message->type,sizeof(message->type));
//	printf("send token %d \n",message->token);
	sendData(sockfd,&message->token,sizeof(message->token));
		
	n=recv(sockfd,&mes_,sizeof(mes_),0);
	printf("get answer %d\n",n);
	if (n<0)
		return 0;
	else
		if (n>0){
			close(sockfd);
			return n;
		}
	return 0;
}


static int checkRoomStatus(room * r){
	float Dt=0.1;
	struct {char type;int token;} message;
	memset(&message,0,sizeof(message));
//	printf("room tok= %d status= %d\n",r->token,r->status);
	if (r->status==ROOM_PREPARE){
		if (r->timer>0)
			r->timer-=Dt;
		else{
			message.type='n'; //'n' for get new room
			message.token=r->token;
			//ask for room $_$
			int i;
			int * $_$;
			int _$_;
			printf("try to get room\n");
			//try to find free server
			//TODO: get sort array of servers
			$_$=serversGetSort();
			for(i=1;i<=*$_$;i++)
				if ((_$_=connSendRecv(serverGetById($_$[i]),
							serversGetPortById($_$[i]),
							&message,
							sizeof(message)))<=0){//we do not get answer, all ok
					//printf("_$_= %d\n",_$_);
					if (_$_<0){//cant connect
						serversSetFail($_$[i]);
					}else{//all ok
						r->status=ROOM_WAIT;
						r->server=$_$[i];
						return 0;
					}
				}
				//we get answer from all servers, cant create room
				printf("cant get room\n");
			r->status=ROOM_FAIL;
			return 0;
		}
	}
	if (r->status==ROOM_ERROR){
		//ask for room
	}
	if (r->status==ROOM_FAIL){
		//room is free
		//if (r->users==0){
		room* _r;
		//remove room
		printf("r->  %d  %d %d\n",r->id,r->type,r->token);
		semop(config.serverworker.room_sem,&sem[0],1);
		_r=roomRem(r->type,r->id);
		semop(config.serverworker.room_sem,&sem[1],1);
		if (_r!=0){
			printf("room removed\n");
			free(_r);
		}
		//}
	}
	return 0;
}

static int proceedServerMessage(worklist* w,char msg_type){
//	char msg;
	short l_l;
	int token;
	room * r_r;
	event * e_e;
	if (msg_type==MESSAGE_ROOM_STATUS){ //packet [mes(char)token(int)status(short)]
		recvData(w->sock,&token,sizeof(token));
		semop(config.serverworker.room_sem,&sem[0],1);
		r_r=roomGetByToken(token);
		semop(config.serverworker.room_sem,&sem[1],1);
//		printf("room\n");
		if (r_r==0)
			return 1;
//		printf("need %d\n",sizeof(room->port));
		recvData(w->sock,&r_r->port,sizeof(r_r->port)); //int
//		printf("port -> %hd\n",r_r->port);
		recvData(w->sock,&r_r->status,sizeof(r_r->status)); //short
//		printf("status -> %hd\n",room->status);
		e_e=eventGet(r_r->type);
		if (e_e==0)
			return 1;
		l_l=strlen(e_e->map);
		sendData(w->sock,&l_l,sizeof(l_l));
		sendData(w->sock,e_e->map,l_l);
		r_r->timestamp=time(0);			
		return 0;
	}
	if (msg_type==MESSAGE_ROOM_RESULT){ //packet [mes(char)token(int)playertoken(int) ..
		printf("get room result\n");
		recvData(w->sock,&token,sizeof(token));
		semop(config.serverworker.room_sem,&sem[0],1);
		r_r=roomGetByToken(token);
		semop(config.serverworker.room_sem,&sem[1],1);
		//add another
		if (r_r==0){
			printf("cant find room\n");
			return 1;
		}
		//add some data
		
		//at the end get status
		recvData(w->sock,&r_r->status,sizeof(r_r->status)); //short
//		printf("get n set r status %d\n",r_r->status);
		return 0;
	}
//	printf("end of mes recv\n");
	return 0;
}

static int recvServerData(worklist* w){
	int i;
	char msg_type;
//	player_info * pl=w->data;
	//try to read message from server 2 times
	for(i=0;i<2;i++){
//		msg_type=0;
		if (recv(w->sock,&msg_type,sizeof(msg_type),MSG_DONTWAIT)<=0){
			//have error check what is it
			if (errno==EAGAIN){
				sleep(0);
				continue;
			}else{
				perror("recv threadServerWorker");
				return 1;
			}
		}
		//get message need to proceed
		if (proceedServerMessage(w,msg_type)!=0)
			return 1;
	}
	
	return 0;
}


void * threadServerWorker(void * arg){
	int id=*(int*)arg;
//	int i;
	worklist * tmp;
	int TPS=10;  //ticks per sec
	struct timeval tv={0,0};
	timePassed(&tv);
	free(arg);
	serversLoad();
	printf("ServerWorker %d started\n",id);
	
	while(config.run){
		tmp=&config.serverworker.client;
		semop(config.serverworker.sem,&sem[0],1);
			for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
				//get data from server about players and statistics
				if (recvServerData(tmp)!=0){
					printf("del serv from list\n");
					//close socket
					close(tmp->sock);
					//delete client
					tmp=worklistDel(&config.serverworker.client,tmp->id);
				}
				
			}
		semop(config.serverworker.sem,&sem[1],1);
			
		semop(config.serverworker.room_sem,&sem[1],1);
		roomCheckAll(checkRoomStatus);
		semop(config.serverworker.room_sem,&sem[0],1);
		
		//some work
		syncTPS(timePassed(&tv),TPS);
	}
	printf("close ServerWorker\n");
	return 0;
}

pthread_t  startServerWorker(int id){
	pthread_t th=0;
	int * arg;
	
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startServerWorker");
	*arg=id;
	
	if((config.serverworker.sem=semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	semop(config.serverworker.sem,&sem[1],1);
	
	if((config.serverworker.room_sem=semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	semop(config.serverworker.room_sem,&sem[1],1);
	
	if(pthread_create(&th,0,threadServerWorker,arg)!=0)
		return 0;
	return th;
}

