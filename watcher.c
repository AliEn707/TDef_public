#include "headers.h"

/*
╔══════════════════════════════════════════════════════════════╗
║	 thread watch for players  take auth add and del them		 ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

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


int clientCheck(worklist * client){
	int worker;
	player_info * pl;
	//new connect client
	if (client->id==0){
		printf("check new client\n");
		//check auth
		pl=dbAuth(client);
		//new client, not reconnect
		if (pl!=0){
			worklist * tmp;
			printf("auth ok\n");
			worker=getFreeWorker();
			//set player data, get from base
			setMask(pl->bitmask,BM_PLAYER_CONNECTED);
			client->data=pl;
			config.worker[worker].client_num++;
			//add player to config.player.tree
			semop(config.player.sem,&sem[0],1);
				if (bintreeAdd(&config.player.tree,client->id,pl)==0){
					perror("bintreeAdd player_info");
					close(client->sock);
					client->id=delPlayerId(client->id);
				}else{
					semop(config.worker[worker].sem,&sem[0],1);
						//add player to worklist of worker thread
						tmp=worklistAdd(&config.worker[worker].client,0);
						tmp->id=client->id;
						tmp->sock=client->sock;
						tmp->data=client->data;
						//keep player data in list to check in future
					semop(config.worker[worker].sem,&sem[1],1);
				}
			semop(config.player.sem,&sem[1],1);
		} else {
			printf("bad auth\n");
			return 1;
		}
	} else{
		//check connected client
		pl=client->data;
		if (pl->conn==CONNECTED){
			//all ok need to check client
		}else{
			//TODO: add counter
			printf("problem with status\n");
			return -1;
		}
	}
	return 0;
}

void * threadWatcher(void * arg){
	int id=*(int*)arg;
	worklist * tmp;
	int TPS=10;  //ticks per sec
	struct timeval tv={0,0};
	timePassed(&tv);
	free(arg);
	usleep(100);
	printf("Watcher %d started\n",id);
	
	while(config.run){
		tmp=&config.watcher.client;
		semop(config.watcher.sem,&sem[0],1);
			for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
				if (clientCheck(tmp)!=0){
					bintreeDel(&config.player.tree,tmp->id);
					free(tmp->data);
					tmp=worklistDel(&config.watcher.client,tmp->id);
					printf("watcher del client\n");
				}
				//some work
			}
		semop(config.watcher.sem,&sem[1],1);
		//some work
		syncTPS(timePassed(&tv),TPS);
	}
	printf("close Watcher\n");
	return 0;
}

pthread_t  startWatcher(int id){
	pthread_t th=0;
	int * arg;
//	struct sembuf sem={0,1,0};
	
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startWatcher");
	*arg=id;
	
	if((config.watcher.sem=semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	semop(config.watcher.sem,&sem[1],1);
	
	if(pthread_create(&th,0,threadWatcher,arg)!=0)
		return 0;
	return th;
}

