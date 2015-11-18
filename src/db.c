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
	//create player info struct
	if ((pl=newPlayer())==0){
		//not have ids
//		close(client->sock);
		return 0;
	}
	//we malloc client need to avoid memmory leak
	do{
//		client->id=rand();
		//set client id  to 1 for future check if some errors will occur
		client->id=1;
		
		pl->bitmask=0;
//		pl->status=PLAYER_CONNECTED;
		pl->conn=CONNECTED;
		//get name 
		$name=rand();	
		//send salt
		if (send(client->sock,&$name,sizeof($name),MSG_NOSIGNAL)<=0)
			break;
//		printf("prepare to get data\n");
		//get name size
		if (recvData(client->sock,&$name,sizeof($name))<=0)
			break;
		printf("got size of name %d\n",$name);
		if ($name!=0){
			if (recvData(client->sock,name,$name)<=0)
				break;
			printf("got name %s\n",name);
			if (recvData(client->sock,&token,sizeof(token))<=0)
				break;
			printf("got token %d\n",token);
			//check token
			//dbSelectWhereNewer(char* table, char* field, char* cmp, char* value, time_t timestamp){
		}else {
			//something strange
			//set first char of name to 0 if error 
			*name=0;
		}
		//check auth
		//& get data from db
		if (dbGetPlayer(pl, name, token)!=0){
			break;
		}
		client->id=pl->id;
		if (pl->id==0)
			break;
		//send client id (the same as player id)
		if (send(client->sock,&client->id,sizeof(client->id),MSG_NOSIGNAL)<=0)
			break;
		double timestamp=time(0);//ActionScript doesn't have int64... used double instead
		if (send(client->sock,&timestamp,sizeof(timestamp),MSG_NOSIGNAL)<=0)
			break;
		return pl;
	}while(0);
	//cause some errors cleanup player info
	realizePlayer(pl);
	return 0;
}

int dbGetPlayer(player_info * pl, char * n, int t){
	if (n==0)
		return -1;
	if (*n==0) //name can't be empty
		return -1;
	//get and set player data
	
	//add check players from wacher list
	
	//dbSelectWhereNewer(char* table, char* field, char* cmp, char* value, time_t timestamp)
	char name[20];
	int user_id;
	sprintf(name,"'%s'",n);
	t_semop(t_sem.db,&sem[0],1);
	dbSelectFieldWhere("users", "id", "email", "=", name);
	if (pgRows()>0){
		int data=pgNumber("id");
		user_id=atoi(pgValue(0,data));
		//select au.* from tdef_player_auths au where ( player_id in (select id from tdef_players pl where user_id = id) ); 
		//dbSelectFieldWhereNewer(char* table, char* sel, char* field, char* cmp, char* value, time_t timestamp)
		char cmp[50];
		sprintf(cmp,"player_id in (select id from tdef_players where user_id = %d)", user_id);
		dbSelectFieldWhereNewer("tdef_player_auths au", "au.*", "", cmp, "", time(0)-5);
		if (pgRows()>0){
			int player_id;
			data=pgNumber("player_id");
			//set player id
			player_id=atoi(pgValue(0,data));
			data=pgNumber("token");
			if (t==atoi(pgValue(0,data))){
				//remove auth entry
				dbUpdateStart("tdef_player_auths");
				dbUpdateValue("token","NULL");
				sprintf(cmp,"(player_id = %d)", pl->id);
				dbUpdateEnd(cmp, 1);
				
				//set player properties
				pl->id=player_id;
				//add another values
			}
			//write to log
			dbLog(player_id, "'login'", 0, "NULL", 0, (pl->id==0 ? "'login error'" : "'login success'") );
		}
	}
	t_semop(t_sem.db,&sem[1],1);
	return 0;
}

int dbGetServer(){
	return 0;
}

int dbFillServers(){
	return 0;
}

int dbFillEvents(){
	//do some stuff
	event * e_e;
	struct sembuf _sem[2]={{0,-WORKER_NUM,0},{0,WORKER_NUM,0}};
	
	t_semop(t_sem.sheduller,&_sem[0],1);
		e_e=eventAdd(1);
		sprintf(e_e->map,"pvz11_11");
		sprintf(e_e->name,"#event");
		//set timestamp;
		config.events.timestamp=time(0);
	t_semop(t_sem.sheduller,&_sem[1],1);
	return 0;
}

/*
dbtime 2015-08-07 10:01:42.565979
linux time Wed Sep 16 08:31:56 2015
*/
char* dbTime(time_t t){
	time_t _t=t;
	static char s[30]; 
//	strftime(s, 20, "%F %H:%M:%S", gmtime(&_t));
	sprintf(s,"'%s'",asctime(gmtime(&_t)));
	return s;
}

time_t dbRawTime(char* s){
	struct tm t;
	time_t ret;
	int error=sscanf(s,"%d-%d-%d %d:%d:%d.%d",&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec,&error);
	t.tm_mon-=1;
	t.tm_year-=1900;
	char *tz= getenv("TZ");
	setenv("TZ", "", 1);
	tzset();
	ret = mktime(&t);
	if (tz)
		setenv("TZ", tz, 1);
	else
		unsetenv("TZ");
	tzset();
	return ret;
}

///database selects 
static char str[500];

int dbSelect(char* table){
	sprintf(str,"SELECT * FROM %s;", table);
	return pgExec(str);
}

int dbSelectWhereUni(char* table, char* fields, char* cmp){
	sprintf(str,"SELECT %s FROM %s where (%s);", fields, table, cmp);
	return pgExec(str);
}

int dbSelectWhere(char* table, char* field, char* cmp, char* value){
	sprintf(str,"SELECT * FROM %s WHERE (%s %s %s);", table, field, cmp, value);
	return pgExec(str);
}

int dbSelectWhereNewer(char* table, char* field, char* cmp, char* value, time_t timestamp){
//	printf("SELECT * FROM %s WHERE (%s %s %s and updated_at > '%s');\n", table, field,cmp,value, dbTime(timestamp)); //can be used for logging
	sprintf(str,"SELECT * FROM %s WHERE (%s %s %s and updated_at > %s);", table, field,cmp,value, dbTime(timestamp));
	return pgExec(str);
}

int dbSelectField(char* table, char* field){
	sprintf(str,"SELECT %s FROM %s;", field, table);
	return pgExec(str);
}

int dbSelectFieldWhere(char* table, char* sel, char* field, char* cmp, char* value){
	sprintf(str,"SELECT %s FROM %s WHERE (%s %s %s);", sel, table, field, cmp, value);
	return pgExec(str);
}

int dbSelectFieldWhereNewer(char* table, char* sel, char* field, char* cmp, char* value, time_t timestamp){
	sprintf(str,"SELECT %s FROM %s WHERE (%s %s %s and updated_at > %s);", sel, table, field, cmp, value, dbTime(timestamp));
	return pgExec(str);
}

int dbSelectNewer(char* table, time_t timestamp){
	sprintf(str,"SELECT * FROM %s WHERE updated_at > %s;", table, dbTime(timestamp));
	return pgExec(str);
}

int dbSelectFieldNewer(char* table,char* field, time_t timestamp){
	sprintf(str,"SELECT %s FROM %s WHERE updated_at > %s;", field, table, dbTime(timestamp));
	return pgExec(str);
}

static struct {
	char table[50];
	char values[2000];
	char tmp[2000];
} qeueing;

//updating
int dbUpdateStart(char* table){
	sprintf(qeueing.table,"%s", table);
	qeueing.values[0]=0;
	return 0;	
}

int dbUpdateValue(char *field, char *value){
	sprintf(qeueing.tmp,"%s%s %s = %s",qeueing.values, qeueing.values[0]!=0? "," : "",field,value);
	sprintf(qeueing.values,"%s", qeueing.tmp);
	return 0;	
}

int dbUpdateEnd(char* cmp, int touch){
	if (touch)
		dbUpdateValue("updated_at", dbTime(time(0)));
	sprintf(str,"UPDATE %s SET  %s  WHERE %s;", qeueing.table, qeueing.values, cmp);
	return pgExec(str);	
}

//inserting
//one line inserting
int dbInsert(char* table, char * f, char * v){
	static char fields[200], values[200];
	sprintf(fields,"%s, created_at, updated_at", f);
	sprintf(values,"%s, %s, %s", v, dbTime(time(0)), dbTime(time(0)));
	sprintf(str,"INSERT INTO %s ( %s ) VALUES ( %s );", table, fields, values);
	return pgExec(str);	
}

//inserting for more complex data
int dbInsertStart(char* table){
	sprintf(qeueing.table,"%s", table);
	qeueing.values[0]=0;
	qeueing.tmp[0]=0;
	return 0;	
}

int dbInsertValue(char *field, char *value){
	static char tmp[200];
	sprintf(tmp,"%s%s %s",qeueing.tmp, qeueing.tmp[0]!=0? "," : "",field);
	sprintf(qeueing.tmp,"%s", tmp);
	sprintf(tmp,"%s%s %s",qeueing.values, qeueing.values[0]!=0? "," : "",value);
	sprintf(qeueing.values,"%s", tmp);
	return 0;	
}

int dbInsertEnd(){
	dbInsertValue("created_at", dbTime(time(0)));
	dbInsertValue("updated_at", dbTime(time(0)));
	sprintf(str,"INSERT INTO %s ( %s ) VALUES ( %s );", qeueing.table, qeueing.tmp, qeueing.values);
	return pgExec(str);	
}


int dbLog(int player_id, char *action, int object_id, char *object_type, int value, char* other){
	static char tmp[50];
	dbInsertStart("tdef_logs");
	dbInsertValue("action", action);
	if (player_id)
		sprintf(tmp,"%d", player_id);
	else
		sprintf(tmp,"NULL");
	dbInsertValue("player_id", tmp);
	if (object_id)
		sprintf(tmp,"%d", object_id);
	else
		sprintf(tmp,"NULL");
	dbInsertValue("logable_id", tmp);
	dbInsertValue("logable_type", object_type);
	sprintf(tmp,"%d", value);
	dbInsertValue("value", tmp);
	dbInsertValue("other", other);
	
	return dbInsertEnd();	
}

