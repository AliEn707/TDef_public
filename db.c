#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ functions for work with database					                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

#define SIZE_OF_PASSWD 32

//check player auth and create player_info struct or ret 0
player_info * dbAuth(worklist * client){
	player_info * pl;
	int $name;
	char name[50];
	char passwd[50];
	memset(name,0,sizeof(name));
	memset(passwd,0,sizeof(passwd));
	if ((newPlayerId())<0){
		//not have ids
		close(client->sock);
		return 0;
	}
	//?
//	client->id=rand();
	printf("client id = %d\n",client->id);
	if ((pl=malloc(sizeof(player_info)))==0){
		perror("malloc player_info");
		close(client->sock);
		client->id=delPlayerId(client->id);
		return 0;
	}
	memset(pl,0,sizeof(player_info));
	
	pl->bitmask=0;
//	pl->status=PLAYER_CONNECTED;
	pl->conn=CONNECTED;
//	pl->id=client->id;
	//get name 
	$name=rand();
	//send salt
	if (send(client->sock,&$name,sizeof($name),MSG_NOSIGNAL)<=0)
		return 0;
//	printf("prepare to get data\n");
	//get name size
	if (recvData(client->sock,&$name,sizeof($name))<=0)
		return 0;
	printf("get size of name %d\n",$name);
	if ($name!=0){
		if (recvData(client->sock,name,$name)<=0) //change to anblock try to get
			return 0;
		printf("get name %s\n",name);
		if (recvData(client->sock,passwd,SIZE_OF_PASSWD)<=0)
			return 0;
		printf("get passwd %s\n",passwd);
	}else {
		//something strange
		//set first char of name to 0 if error 
		*name=0;
	}
	//check auth
	//& get data from db
	if (dbGetPlayer(pl, name)!=0){
		close(client->sock);
		client->id=delPlayerId(client->id);
		return 0;
	}
	client->id=pl->id;
	//send client id
	if (send(client->sock,&client->id,sizeof(client->id),MSG_NOSIGNAL)<=0)
		return 0;
	return pl;
}

int dbGetPlayer(player_info * pl, char * name){
	if (name==0)
		return -1;
	//get and set player data
	pl->id=rand();
	return 0;
}

int dbGetServer(){
	return 0;
}

int dbFillServers(){
	return 0;
}

static int events_timestamp=0; 
int dbFillEvents(){
	//do some stuff
	event * e_e;
	e_e=eventAdd(1);
	sprintf(e_e->map,"4");
	events_timestamp=time(0);
	return 0;
}

