﻿#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ thread work with held tasks					                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

struct{
	char str[30];
}* map_names;

static inline int proceedUpdaterMessage(worklist* w,char msg_type){
	int i;
	char t_t[100];
	int len;
	char* value;
	int rows;
	long long int timestamp;
	if (msg_type==MESSAGE_UPDATE_MAPS){
		printf("update maps\n");
//		t_semop(t_sem.db,&sem[0],1);
		dbQuery_t query;
		query=dbSelectFieldWhere("tdef_maps","name","completed","=","'t'");
		int name=pgNumber(query, "name");
		rows=pgRows(query);
		map_names=malloc(rows*sizeof(*map_names));
		memset(map_names,0,rows*sizeof(*map_names));
		for(i=0;i<rows;i++){
			strcpy(map_names[i].str,pgValue(query, i,name));
		}
		dbClear(query);
//		t_semop(t_sem.db,&sem[1],1);
		for(i=0;i<rows;i++){
			len=strlen(map_names[i].str);
			sendData(w->sock,&len,sizeof(len));
			sendData(w->sock,map_names[i].str,len);
			if(recvData(w->sock,&timestamp,sizeof(timestamp))>0){
				sprintf(t_t,"'%s'",map_names[i].str);
//				t_semop(t_sem.db,&sem[0],1);
				query=dbSelectWhereNewer("tdef_maps","name","=",t_t,timestamp);
				if (pgRows(query)>0){
					int data=pgNumber(query, "data");
					value=pgValue(query, 0,data);
					len=strlen(value);
					sendData(w->sock,&len,sizeof(len));
					sendData(w->sock,value,len);
					sprintf(t_t,"\n");
					len=strlen(t_t);
					sendData(w->sock,&len,sizeof(len));
					sendData(w->sock,t_t,len);
				}
				dbClear(query);
//				t_semop(t_sem.db,&sem[1],1);
			}
			len=0;
			sendData(w->sock,&len,sizeof(len));
		}
		free(map_names);
		len=0;
		sendData(w->sock,&len,sizeof(len));
		return 0;
	}
	if (msg_type==MESSAGE_UPDATE_NPC_TYPES ||
	    msg_type==MESSAGE_UPDATE_TOWER_TYPES ||
	    msg_type==MESSAGE_UPDATE_BULLET_TYPES ){ //packet [mes(char) ..
		char * TYPES[]={
			[MESSAGE_UPDATE_NPC_TYPES]="tdef_type_npcs",
			[MESSAGE_UPDATE_TOWER_TYPES]="tdef_type_towers",
			[MESSAGE_UPDATE_BULLET_TYPES]="tdef_type_bullets"
		};
		printf("update %s\n", TYPES[(int)msg_type]);
		dbQuery_t query;
		recvData(w->sock,&timestamp,sizeof(timestamp));
//		t_semop(t_sem.db,&sem[0],1);
		query=dbSelectFieldNewer(TYPES[(int)msg_type], "id", timestamp);
		rows=pgRows(query);
		dbClear(query);
//		t_semop(t_sem.db,&sem[1],1);
		if (rows!=0){
//			t_semop(t_sem.db,&sem[0],1);
			query=dbSelect(TYPES[(int)msg_type]);
			rows=pgRows(query);
			int params=pgNumber(query,"params");
			int id=pgNumber(query,"id");
			for(i=0;i<rows;i++){
				sprintf(t_t,"id %s\n",pgValue(query, i, id));
				len=strlen(t_t);
				sendData(w->sock,&len,sizeof(len));
				sendData(w->sock,t_t,len);
				value=pgValue(query, i, params);
				len=strlen(value);
				sendData(w->sock,&len,sizeof(len));
				sendData(w->sock,value,len);
				
				sprintf(t_t,"//- %s\n\n",pgValue(query, i, id));
				len=strlen(t_t);
				sendData(w->sock,&len,sizeof(len));
				sendData(w->sock,t_t,len);
			}
			dbClear(query);
//			t_semop(t_sem.db,&sem[1],1);
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
				errno=0;
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
	int TPS=4;  //ticks per sec
	struct timeval tv={0,0};

	void* proceed(worklist* tmp, void* arg){
		//some work
		//send message
		if (recvUpdaterData(tmp)!=0){
			printf("remove updater task\n");
			//close socket
			close(tmp->sock);
			//delete client
			return (void*)1;
		}
		return 0;
	}

	timePassed(&tv);
	free(arg);
	printf("Updater started\n");
	while(config.run){
		//check tasks
		t_semop(t_sem.updater,&sem[0],1);
			//do actions
			worklistForEachRemove(&config.updater.task,proceed,0);
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

