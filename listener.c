#include "headers.h"

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
		perror("bind listene");
		return 0;
	}
	if(listen(listener, 1)<0){
		perror("listen listene");
		return 0;
	}
	
	return listener;
}

int getFreeWorker(){
	int i;
	int min,id=0;
	min=config.worker[0].client_num;
	for(i=1;i<WORKER_NUM;i++)
		if (config.worker[i].client_num<min){
			min=config.worker[i].client_num;
			id=i;
		}
	return id;
}


void * threadListener(void * arg){
	int max_fd;
	struct sockaddr_in addr;
	struct sembuf sem;
	fd_set master;
	fd_set read_fds;
	struct timeval t={0,0};
	int time;
	printf("start Listener\n");
	memset(&sem,0,sizeof(sem));
	
	time=1000/20;
	config.listenet.sock=startServer(config.listener.port);
	config.player.sock=startServer(config.player.port);
	max_fd=config.player.sock>config.listenet.sock?config.player.sock:config.listenet.sock;
	
	while(config.run){
		FD_ZERO(&read_fds);
		FD_SET(config.listenet.sock, &read_fds);
		FD_SET(config.player.sock, &read_fds);
		
		if (select(max_fd+1, &read_fds, 0, 0, 0)){
			if (FD_ISSET(config.listenet.sock, &read_fds){
				
				sleep(0);
			}
			
			if (FD_ISSET(config.player.sock, &read_fds){
				int id;
				int w_id,sock;
				if((sock = accept(config.player.sock, NULL, NULL))<0)
					perror("accept listener");
				if ((id=newPlayerId())<0){
					close(sock);
				}else{
					worker=getFreeWorker();
					config.worker[worker].client_num++;
					//add player to config.player.tree
					//add player to worklist of worker thread
				}
				sleep(0);
			}
		}
//		syncTime(*t,time);
	}
	printf("close Listener\n");
	close(listener);
	return 0;
}

int startListener(){
	pthread_t th=0;
	
	if(pthread_create(&th,0,threadListener,0)!=0)
		return 0;
	return th;
}
