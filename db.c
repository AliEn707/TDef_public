#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ functions for work with database					                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

#define SIZE_OF_PASSWD 16

//check player auth and create player_info struct or ret 0
player_info * dbAuth(worklist * client){
	player_info * pl;
	int name$;
	char name[50];
	char passwd[50];
	memset(name,0,sizeof(name));
	memset(passwd,0,sizeof(passwd));
	if ((client->id=newPlayerId())<0){
		//not have ids
		close(client->sock);
		return 0;
	}
	if ((pl=malloc(sizeof(player_info)))==0){
		perror("malloc player_info");
		close(client->sock);
		client->id=delPlayerId(client->id);
		return 0;
	}
	memset(pl,0,sizeof(player_info));
	
	pl->bitmask=0;
	pl->status=PLAYER_CONNECTED;
	pl->conn=CONNECTED;
//	pl->id=client->id;
	//get name 
	name$=rand();
	//send salt
	send(client->sock,&name$,sizeof(name$),MSG_NOSIGNAL);//short
	
	//get name size
	if (recvData(client->sock,&name$,sizeof(name$))<0)
		return 0;
	printf("get size of name %d",name$);
	if (name$!=0){
		if (recvData(client->sock,name,name$)<0) //change to anblock try to get
			return 0;
		printf("get name %s\n",name);
		if (recvData(client->sock,passwd,SIZE_OF_PASSWD)<0)
			return 0;
		printf("get passwd %s\n",passwd);
	}else
		*name=0;
	//check auth
	if (0){
		close(client->sock);
		client->id=delPlayerId(client->id);
		return 0;
	}
	//get data from db
	if (gbGetPlayer(pl, name)!=0){
		close(client->sock);
		client->id=delPlayerId(client->id);
		return 0;
	}
	
	return pl;
}

int gbGetPlayer(player_info * pl, char * name){
	if (name==0)
		return -1;
	//get and set player data
	
	return 0;
}

int dbGetServer(){
	return 0;
}

int dbFillServers(){
	return 0;
}

