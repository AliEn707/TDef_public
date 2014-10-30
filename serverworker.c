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
	if (hostname==0)
		return -1;
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
	memcpy((char *)server->h_addr,(char *)&servaddr.sin_addr.s_addr, server->h_length);
//	servaddr.sin_addr.s_addr=inet_addr("172.16.1.40");//argv[1]);
	servaddr.sin_port=htons(port);

	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
		perror("connect");
		return -2;
	}	
	
	sendData(sockfd,mes,$mes);
		
	n=recv(sockfd,&mes_,sizeof(mes_),0);
	if (n<0)
		return -1;
	else
		if (n>0)
			return n;
	return 0;
}


static int checkRoomStatus(room * r){
	float Dt=0.1;
	struct {char type;int token;} message;
	memset(&message,0,sizeof(message));
	if(r->stat>0){
		if (r->status==ROOM_PREPARE){
			if (r->timer>0)
				r->timer-=Dt;
			else{
				message.type='n'; //'n' for get new room
				message.token=r->token;
				//ask for room $_$
				int $_$;
				int _$_;
				printf("try to get room\n");
				//try to find free server
				for($_$=serversGetNum();$_$>0;$_$--)
					if ((_$_=connSendRecv(serverGetById($_$),
								serversGetPortById($_$),
								&message,
								sizeof(message)))<=0){
						if (_$_<0){
							serversSetFail($_$);
						}else{
							return 0;
						}
					}
					printf("cant get room\n");
				r->status=ROOM_FAIL;
				return 0;
			}
		}
		if (r->status==ROOM_ERROR){
			//ask for room
		}
		if (r->status==ROOM_FAIL){
			//mark for del
			r->stat=-1;
		}
	}
	//room need to del
	if(r->stat<0){
		//room is free
		if (r->users==0){
			room* _r;
			//remove room
			_r=roomRem(r->type,r->id);
			if (_r!=0){
				printf("room removed\n");
				free(_r);
			}
		}
	}
	return 0;
}

static int proceedServerMessage(worklist* w,char msg_type){
//	char msg;
	if (msg_type==MESSAGE_ROOM_STATUS){ //packet [mes(char)token(int)status(short)]
		int token;
		room * room;
		recvData(w->sock,&token,sizeof(token));
		room=roomGetByToken(token);
		recvData(w->sock,&room->status,sizeof(room->status)); //short
	}
	if (msg_type==MESSAGE_ROOM_RESULT){ //packet [mes(char)token(int)playertoken(int) ..
		int token;
		room * room;
		recvData(w->sock,&token,sizeof(token));
		room=roomGetByToken(token);
		recvData(w->sock,&token,sizeof(token));
		//add another
		
		//del after
		int $_$=0;
		$_$=room->users+$_$;
	}
	return 0;
}

static int recvServerData(worklist* w){
	int i;
	char msg_type;
//	player_info * pl=w->data;
	//try to read message from server 2 times
	for(i=0;i<2;i++){
		if (recv(w->sock,&msg_type,sizeof(msg_type),MSG_DONTWAIT)<0){
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
				recvServerData(tmp);
				//close socket
				//delete client
			}
		semop(config.serverworker.sem,&sem[1],1);
		roomCheckAll(checkRoomStatus);
		
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
	
	if(pthread_create(&th,0,threadServerWorker,arg)!=0)
		return 0;
	return th;
}

