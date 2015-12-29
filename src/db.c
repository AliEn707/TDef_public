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

//clear query
void dbClear(dbQuery_t id){
	pgClear(id);
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
	//we malloc client, need to avoid memory leak
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
	dbQuery_t query1=0, query2=0;
	sprintf(name,"'%s'",n);
//	t_semop(t_sem.db,&sem[0],1);
	query1=dbSelectFieldWhere("users", "id", "email", "=", name);
	if (pgRows(query1)>0){
		int data=pgNumber(query1, "id");
		user_id=atoi(pgValue(query1, 0,data));
		//select au.* from tdef_player_auths au where ( player_id in (select id from tdef_players pl where user_id = id) ); 
		//dbSelectFieldWhereNewer(char* table, char* sel, char* field, char* cmp, char* value, time_t timestamp)
		char cmp[50];
		sprintf(cmp,"player_id in (select id from tdef_players where user_id = %d)", user_id);
		query2=dbSelectFieldWhereNewer("tdef_player_auths au", "au.*", "", cmp, "", time(0)-50);
		if (pgRows(query2)>0){
			int player_id;
			data=pgNumber(query2, "player_id");
			//set player id
			player_id=atoi(pgValue(query2, 0,data));
			data=pgNumber(query2, "token");
			if (t==atoi(pgValue(query2, 0,data))){
				dbUpdate_t u_u;
				//remove auth entry
				u_u=dbUpdateStart("tdef_player_auths");
				dbUpdateValue(u_u, "token","NULL");
				sprintf(cmp,"(player_id = %d)", pl->id);
				dbUpdateEnd(u_u, cmp, 1);
				//set player properties
				pl->id=player_id;
				//add another values
			}
			//write to log
			dbLog(player_id, "'login'", 0, "NULL", 0, (pl->id==0 ? "'login error'" : "'login success'") );
		}
	}
	dbClear(query1);
	dbClear(query2);
//	t_semop(t_sem.db,&sem[1],1);
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
	//set semaphore to busy
	t_semop(t_sem.events,&_sem[0],1);
		eventErase();
		e_e=eventAdd(1);
		sprintf(e_e->map,"pvz11_11");
		sprintf(e_e->name,"#event");
		//set timestamp;
		config.events.timestamp=time(0);
	t_semop(t_sem.events,&_sem[1],1);
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

dbQuery_t dbSelect(char* table){
	char str[500];
	sprintf(str,"SELECT * FROM %s;", table);
	return pgExec(str);
}

dbQuery_t dbSelectWhereUni(char* table, char* fields, char* cmp){
	char str[500];
	sprintf(str,"SELECT %s FROM %s where (%s);", fields, table, cmp);
	return pgExec(str);
}

dbQuery_t dbSelectWhere(char* table, char* field, char* cmp, char* value){
	char str[500];
	sprintf(str,"SELECT * FROM %s WHERE (%s %s %s);", table, field, cmp, value);
	return pgExec(str);
}

dbQuery_t dbSelectWhereNewer(char* table, char* field, char* cmp, char* value, time_t timestamp){
//	printf("SELECT * FROM %s WHERE (%s %s %s and updated_at > '%s');\n", table, field,cmp,value, dbTime(timestamp)); //can be used for logging
	char str[500];
	sprintf(str,"SELECT * FROM %s WHERE (%s %s %s and updated_at > %s);", table, field,cmp,value, dbTime(timestamp));
	return pgExec(str);
}

dbQuery_t dbSelectField(char* table, char* field){
	char str[500];
	sprintf(str,"SELECT %s FROM %s;", field, table);
	return pgExec(str);
}

dbQuery_t dbSelectFieldWhere(char* table, char* sel, char* field, char* cmp, char* value){
	char str[500];
	sprintf(str,"SELECT %s FROM %s WHERE (%s %s %s);", sel, table, field, cmp, value);
	return pgExec(str);
}

dbQuery_t dbSelectFieldWhereNewer(char* table, char* sel, char* field, char* cmp, char* value, time_t timestamp){
	char str[500];
	sprintf(str,"SELECT %s FROM %s WHERE (%s %s %s and updated_at > %s);", sel, table, field, cmp, value, dbTime(timestamp));
	return pgExec(str);
}

dbQuery_t dbSelectNewer(char* table, time_t timestamp){
	char str[500];
	sprintf(str,"SELECT * FROM %s WHERE updated_at > %s;", table, dbTime(timestamp));
	return pgExec(str);
}

dbQuery_t dbSelectFieldNewer(char* table,char* field, time_t timestamp){
	char str[500];
	sprintf(str,"SELECT %s FROM %s WHERE updated_at > %s;", field, table, dbTime(timestamp));
	return pgExec(str);
}

#define MAX_QEUEING 20
static struct {
	short used;
	char table[50];
	char values[2000];
	char tmp[2000];
} qeueing[MAX_QEUEING];

static inline dbUpdate_t getAccess(){
	dbUpdate_t id=0;
	int i;
	do {
		t_semop(t_sem.db,&sem[0],1);
			for(i=0;i<MAX_QEUEING;i++)
				if (qeueing[i].used==0){
					id=i;
				}
		t_semop(t_sem.db,&sem[1],1);
		if (id!=0)
			break;
		usleep(30000);
	}while(1);
	return id;
}

static inline void dropAccess(dbUpdate_t id){
	t_semop(t_sem.db,&sem[0],1);
		qeueing[id].used=0;
	t_semop(t_sem.db,&sem[1],1);
}


//updating
dbUpdate_t dbUpdateStart(char* table){
	dbUpdate_t id=getAccess();
	sprintf(qeueing[id].table,"%s", table);
	qeueing[id].values[0]=0;
	return id;	
}

void dbUpdateValue(dbUpdate_t id, char *field, char *value){
	sprintf(qeueing[id].tmp,"%s%s %s = %s",qeueing[id].values, qeueing[id].values[0]!=0? "," : "",field,value);
	sprintf(qeueing[id].values,"%s", qeueing[id].tmp);
}

void dbUpdateEnd(dbUpdate_t id, char* cmp, int touch){
	char str[500];
	if (touch)
		dbUpdateValue(id, "updated_at", dbTime(time(0)));
	sprintf(str,"UPDATE %s SET  %s  WHERE %s;", qeueing[id].table, qeueing[id].values, cmp);
	pgClear(pgExec(str));	
	dropAccess(id);
}

//inserting
//one line inserting
void dbInsert(char* table, char * f, char * v){ //f- fields : "qq,ww,ee", v - values "''qq', 22, 46"
	char fields[200], values[200], str[500];
	sprintf(fields,"%s, created_at, updated_at", f);
	sprintf(values,"%s, %s, %s", v, dbTime(time(0)), dbTime(time(0)));
	sprintf(str,"INSERT INTO %s ( %s ) VALUES ( %s );", table, fields, values);
	dbClear(pgExec(str));
}

//inserting for more complex data
dbUpdate_t dbInsertStart(char* table){
	dbUpdate_t id=getAccess();
	sprintf(qeueing[id].table,"%s", table);
	qeueing[id].values[0]=0;
	qeueing[id].tmp[0]=0;
	return id;	
}

void dbInsertValue(dbUpdate_t id, char *field, char *value){
	static char tmp[200];
	sprintf(tmp,"%s%s %s",qeueing[id].tmp, qeueing[id].tmp[0]!=0? "," : "",field);
	sprintf(qeueing[id].tmp,"%s", tmp);
	sprintf(tmp,"%s%s %s",qeueing[id].values, qeueing[id].values[0]!=0? "," : "",value);
	sprintf(qeueing[id].values,"%s", tmp);
}

void dbInsertEnd(dbUpdate_t id){
	char str[500];
	dbInsertValue(id, "created_at", dbTime(time(0)));
	dbInsertValue(id, "updated_at", dbTime(time(0)));
	sprintf(str,"INSERT INTO %s ( %s ) VALUES ( %s );", qeueing[id].table, qeueing[id].tmp, qeueing[id].values);
	dbClear(pgExec(str));
}


void dbLog(int player_id, char *action, int object_id, char *object_type, int value, char* other){
	char tmp[50];
	dbUpdate_t id;
	id=dbInsertStart("tdef_logs");
	dbInsertValue(id, "action", action);
	if (player_id)
		sprintf(tmp,"%d", player_id);
	else
		sprintf(tmp,"NULL");
	dbInsertValue(id, "player_id", tmp);
	if (object_id)
		sprintf(tmp,"%d", object_id);
	else
		sprintf(tmp,"NULL");
	dbInsertValue(id, "logable_id", tmp);
	dbInsertValue(id, "logable_type", object_type);
	sprintf(tmp,"%d", value);
	dbInsertValue(id, "value", tmp);
	dbInsertValue(id, "other", other);
	
	dbInsertEnd(id);	
}

