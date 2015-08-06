#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with held tasks					                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

static inline int proceedUpdaterMessage(worklist* w,char msg_type){
	int i;
	char t_t[100];
	int len;
	if (msg_type==MESSAGE_UPDATE_NPC_TYPES){ //packet [mes(char) ..
		int timestamp;
		recvData(w->sock,&timestamp,sizeof(timestamp));
		t_semop(t_sem.db,&sem[0],1);
			dbGetNpcTypes(timestamp);
			int rows=pgRows();
			int params=pgNumber("params");
			int id=pgNumber("id");
			for(i=0;i<rows;i++){
				char* value=pgValue(i,id);
				sprintf(t_t,"id %s\n",value);
				len=strlen(t_t);
				sendData(w->sock,&len,sizeof(len));
				sendData(w->sock,t_t,len);
				value=pgValue(i,params);
				len=strlen(value);
				sendData(w->sock,&len,sizeof(len));
				sendData(w->sock,value,len);
				t_semop(t_sem.db,&sem[1],1);
				sprintf(t_t,"//-\n\n");
				len=strlen(t_t);
				sendData(w->sock,&len,sizeof(len));
				sendData(w->sock,t_t,len);
			}
		len=0;
		sendData(w->sock,&len,sizeof(len));
		return 0;
	}
	return 0;
}

static inline int recvUpdaterData(worklist* w){
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
				perror("recv threadUpdater");
				return 1;
			}
		}
		//get message need to proceed
		if (proceedUpdaterMessage(w,msg_type)!=0)
			return 1;
	}
	
	return 0;
}

void * threadUpdater(void * arg){
	worklist * tmp;
	int TPS=4;  //ticks per sec
	struct timeval tv={0,0};
	timePassed(&tv);
	free(arg);
	printf("Updater started\n");
	while(config.run){
		//check tasks
		tmp=&config.updater.task;
		t_semop(t_sem.updater,&sem[0],1);
			for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
				//some work
				//send message
				if (recvUpdaterData(tmp)!=0){
					printf("remove updater task\n");
					//close socket
					close(tmp->sock);
					//delete client
					tmp=worklistDel(&config.updater.task,tmp->id);
				}
			}
		t_semop(t_sem.updater,&sem[1],1);
		
		syncTPS(timePassed(&tv),TPS);
	}
	printf("close Updater\n");
	return 0;
}

pthread_t startUpdater(){
	pthread_t th=0;
	int * arg;
	
	if ((arg=malloc(sizeof(int)))==0)
		perror("malloc startUpdater");
	*arg=0;
	
	if((t_sem.updater=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	t_semop(t_sem.updater,&sem[1],1);
	
	if((t_sem.db=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		return 0;
	t_semop(t_sem.db,&sem[1],1);
	
	if(pthread_create(&th,0,threadUpdater,arg)!=0)
		return 0;
	return th;
}

