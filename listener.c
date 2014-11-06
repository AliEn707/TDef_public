#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread listen sockets 							                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


int startServer(port){
	int listener;
	struct sockaddr_in addr;
	
	listener=socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int)) == -1){
		perror("setsockopt() listener");
		return 0;	
	}
	
	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind listener");
		return 0;
	}
	if(listen(listener, 1)<0){
		perror("listen listener");
		return 0;
	}
	
	return listener;
}

void * threadListener(void * arg){
	int max_fd;
//	struct sockaddr_in addr;
//	fd_set master;
	fd_set read_fds;
	int TPS=10;  //ticks per sec
	struct timeval tv={0,0};
	timePassed(&tv);
	
	usleep(100);
	printf("Listener started\n");
//	memset(&sem,0,sizeof(sem));
	
//	time=1000/20;
	//get sockets
	config.serverworker.sock=startServer(config.serverworker.port);
	config.player.sock=startServer(config.player.port);
	//need for select
	max_fd=config.player.sock>config.serverworker.sock?config.player.sock:config.serverworker.sock;
	while(config.run){
		FD_ZERO(&read_fds);
		FD_SET(config.serverworker.sock, &read_fds);
		FD_SET(config.player.sock, &read_fds);
		
		if (select(max_fd+1, &read_fds, 0, 0, 0)){
			if (FD_ISSET(config.serverworker.sock, &read_fds)){
				int sock;
				worklist * tmp;
				if((sock = accept(config.serverworker.sock, NULL, NULL))<0)
					perror("accept listener");
				config.serverworker.client_num++;
				printf("conn server\n");
				semop(config.serverworker.sem,&sem[0],1);
					//add client to serverworker
					tmp=worklistAdd(&config.serverworker.client,0);
					tmp->sock=sock;
				semop(config.serverworker.sem,&sem[1],1);
				sleep(0);
			}
			
			if (FD_ISSET(config.player.sock, &read_fds)){
//				int w_id;
				int sock;
				worklist * tmp;
				if((sock = accept(config.player.sock, NULL, NULL))<0)
					perror("accept listener");
				printf("player connected\n");
				config.watcher.client_num++;
//				printf("semval= %d\n",semctl(config.watcher.sem,1,GETVAL));
				semop(config.watcher.sem,&sem[0],1);
					//add client to watcher
					if ((tmp=worklistAdd(&config.watcher.client,0))==0){
						perror("worklistAdd Listener");
						close(sock);
					} else {
						tmp->sock=sock;
//						printf("added to watcher\n");
					}
				semop(config.watcher.sem,&sem[1],1);
				sleep(0);
			}
		}
		syncTPS(timePassed(&tv),TPS);
//		syncTime(*t,time);
	}
	printf("close Listener\n");
	return 0;
}

pthread_t startListener(){
	pthread_t th=0;
	if((config.player.sem=semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	semop(config.player.sem,&sem[1],1);
	
//	if((config.serverworker.sem=semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
//		return 0;
//	semop(config.serverworker.sem,&sem[1],1);
	
	if(pthread_create(&th,0,threadListener,0)!=0)
		return 0;
	return th;
}
