#include <stdio.h>

#include "main.h"
#include "postgres.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ functions to work with postgres db							       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


static PGconn *connection=0;

static dbConnection connections[DB_CONNECTIONS];
//char data[]="host=localhost\ndbname=wss_devel\nuser=dbuser\npassword=passwd";

static inline dbQuery_t getAccess(){
	dbQuery_t id=0;
	int i;
	do {
		t_semop(t_sem.db,&sem[0],1);
			for(i=1;i<DB_CONNECTIONS;i++)
				if (connections[i].used==0){
					id=i;
				}
		t_semop(t_sem.db,&sem[1],1);
		if (id!=0)
			break;
		usleep(30000);
	}while(1);
	return id;
}

static inline void dropAccess(dbQuery_t id){
	t_semop(t_sem.db,&sem[0],1);
		connections[id].used=0;
	t_semop(t_sem.db,&sem[1],1);
}

int pgConnect(char * cparams){
	memset(&connections, 0, sizeof(connections));
	connection=PQconnectdb(cparams);
	return PQstatus(connection); 
}

void pgClose(){
	if (connection==0)
		return;
	PQfinish(connection);
	connection=0;
}

int pgCheck(){
	if (connection==0)
		return 1;
	if (PQstatus(connection)!=0)
		PQreset(connection);
	return PQstatus(connection);
}

void pgClear(dbQuery_t id){
	if (id==0)
		return;
	if (connections[id].query!=0)
			PQclear(connections[id].query);
		connections[id].query=0;
	dropAccess(id);
}

dbQuery_t pgExec(char * query){
	dbQuery_t id=getAccess();
	if (connection==0)
		return 1;
	connections[id].query= PQexec(connection,query);
	pgErrorPrint();
	return id;
}//must be cleared by PQclear


int pgRows(dbQuery_t id){
	if (connection==0)
		return 0;
	if (connections[id].query)
		return PQntuples(connections[id].query);
	else
		return 0;
}

int pgColumns(dbQuery_t id){
	if (connection==0)
		return 0;
	if (connections[id].query)
		return PQnfields(connections[id].query);
	else
		return 0;
}

char* pgError(){
	return PQerrorMessage(connection);
}

void pgErrorPrint(){
	char * str=PQerrorMessage(connection);
	if (str[0]!=0)
		printf("DB Error: %s\n", PQerrorMessage(connection)); //change to log print
}

int pgNumber(dbQuery_t id, char * name){
	if (connection==0)
		return -1;
	return PQfnumber(connections[id].query,name);
}

int pgSize(dbQuery_t id, int row, int col){
	if (connection==0)
		return 0;
	return PQgetlength(connections[id].query, row, col);
}

char *pgValue(dbQuery_t id, int row, int col){
	if (connection==0)
		return "";
	return PQgetvalue(connections[id].query,row,col);
}


/*
int main(){
	pgConnect(data);
	pgExec("select * from users;");
	printf("%s %d\n",pgValue(1,pgNumber("id")), pgNumber("updated_at"));
	printf("%s %d\n",pgValue(1,pgNumber("email")), pgNumber("updated_at"));
	printf("%s %d\n",pgValue(1,pgNumber("name")), pgNumber("updated_at"));
	pgExec("UPDATE users set name = 'test' where email = 'user@test.test';");
	printf("%s",pgError());
	pgExec("select * from users;");
	printf("%s %d\n",pgValue(1,pgNumber("name")), pgNumber("updated_at"));
	
	pgClose();
	return 0;
}
*/
//UPDATE per­sons set street = 'Nis­sesti­en 67', ci­ty = 'Sand­nes' where lastname = 'Tjes­sem' and firs­tna­me = 'Ja­kob';
//INSERT INTO image (name, raster) VALUES ('beautiful image', 'smth');