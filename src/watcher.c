#include "headers.h"

/*
╔══════════════════════════════════════════════════════════════╗
║	 thread watch for players  take auth add and del them		 ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

// !!!!!!!!!! watcher can't ask for listener worklist !!!!!!!!!

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
		printf("client id= %d\n",client->id);
		//new client, not reconnect
		if (pl!=0){
			worklist * tmp;
			printf("auth ok\n");
			worker=getFreeWorker();
			//set player data, get from base
			setMask(pl->bitmask,BM_PLAYER_CONNECTED);
			client->data=pl;
			
			//add player to config.player.tree
			t_semop(t_sem.player,&sem[0],1);
				if (bintreeAdd(&config.player.tree,pl->id,pl)==0){
					perror("bintreeAdd player_info");
//					close(client->sock);
					pl->conn=FAIL;
				}else{
					t_semop(t_sem.worker[worker],&sem[0],1);
						config.worker[worker].client_num++;
						//add player to worklist of worker thread
						tmp=worklistAdd(&config.worker[worker].client, client->id);
						tmp->sock=client->sock;
						tmp->data=client->data;
						//keep player data in list to check in future
					t_semop(t_sem.worker[worker],&sem[1],1);
				}
			t_semop(t_sem.player,&sem[1],1);
		} else {
			printf("bad auth\n");
			return 1;
		}
	}else{
		//check connected client
		if ((pl=client->data)==0)
			return -1;
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
	int TPS=3;  //ticks per sec
	struct timeval tv={0,0};
	
	void* proceed(worklist* tmp, void* arg){
		if (clientCheck(tmp)!=0){
			bintreeDel(&config.player.tree,tmp->id,realizePlayer);//tmp->data == pl
			config.watcher.client_num--;
			close(tmp->sock);
			printf("watcher del client\n");
			return (void*)1;
		}
		//some work
		return 0;
	}
	
	timePassed(&tv);
	free(arg);
	usleep(100);
	printf("Watcher %d started\n",id);
	
	while(config.run){
		t_semop(t_sem.watcher,&sem[0],1);
			//do actions
			worklistForEachRemove(&config.watcher.client,proceed,0);
		t_semop(t_sem.watcher,&sem[1],1);
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
	
	if((t_sem.watcher=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	t_semop(t_sem.watcher,&sem[1],1);
	
	if(pthread_create(&th,0,threadWatcher,arg)!=0)
		return 0;
	
//	prctl(PR_SET_NAME, "TDef public watcher",0,0,0);
	return th;
}

