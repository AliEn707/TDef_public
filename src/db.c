#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ functions for work with database					                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

int dbConnect(char* config){
	FILE*f_f;
	char data[1000];
	if ((f_f=fopen(config,"rt"))==0){
		perror("open database.cfg");
		return -1;
	}
	fread(data,1,sizeof(data),f_f);
	fclose(f_f);
	return pgConnect(data);	
}


//check player auth and create player_info struct or ret 0
player_info * dbAuth(worklist * client){
	player_info * pl;
	int $name;
	char name[50];
	int token;
	memset(name,0,sizeof(name));
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
	//get name 
	$name=rand();
	//send salt
	if (send(client->sock,&$name,sizeof($name),MSG_NOSIGNAL)<=0)
		return 0;
//	printf("prepare to get data\n");
	//get name size
	if (recvData(client->sock,&$name,sizeof($name))<=0)
		return 0;
	printf("got size of name %d\n",$name);
	if ($name!=0){
		if (recvData(client->sock,name,$name)<=0) //change to anblock try to get
			return 0;
		printf("got name %s\n",name);
		if (recvData(client->sock,&token,sizeof(token))<=0)
			return 0;
		printf("got token %d\n",token);
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
	sprintf(e_e->map,"pvz11_11");
	events_timestamp=time(0);
	return 0;
}

char* dbTime(int t){
	time_t _t=t;
	return asctime(gmtime(&_t));
}

///database selects 
static char str[500];

int dbSelect(char* table){
	sprintf(str,"SELECT * FROM %s;", table);
	return pgExec(str);
}

int dbSelectWhere(char* table, char* field, char* cmp, char* value){
	sprintf(str,"SELECT * FROM %s WHERE (%s %s %s);", table, field, cmp, value);
	return pgExec(str);
}

int dbSelectWhereNewer(char* table, char* field, char* cmp, char* value, int timestamp){
//	printf("SELECT * FROM %s WHERE (%s %s %s and updated_at > '%s');\n", table, field,cmp,value, dbTime(timestamp)); //can be used for logging
	sprintf(str,"SELECT * FROM %s WHERE (%s %s %s and updated_at > '%s');", table, field,cmp,value, dbTime(timestamp));
	return pgExec(str);
}

int dbSelectField(char* table, char* field){
	sprintf(str,"SELECT %s FROM %s;", field, table);
	return pgExec(str);
}

int dbSelectNewer(char* table, int timestamp){
	sprintf(str,"SELECT * FROM %s WHERE updated_at > '%s';", table, dbTime(timestamp));
	return pgExec(str);
}

int dbSelectFieldNewer(char* table,char* field, int timestamp){
	sprintf(str,"SELECT %s FROM %s WHERE updated_at > '%s';", field, table, dbTime(timestamp));
	return pgExec(str);
}

