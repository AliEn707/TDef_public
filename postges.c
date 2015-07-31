#include <libpq-fe.h>


#define print_error() printf("%s",PQerrorMessage(connection))

static PGconn *connection=0;

static PGresult * last_result=0;
//char data[]="host=localhost\ndbname=wss_devel\nuser=dbuser\npassword=passwd";

int dbConnect(char * cparams){
	connection=PQconnectdb(cparams);
	return PQstatus(connection); 
}

void dbClose(){
	PQfinish(connection);
}

int dbCheck(){
	if (PQstatus(connection)!=0)
		PQreset(connection);
	return PQstatus(connection);
}

int dbExec(char * query){
	if (last_result!=0) 
		PQclear(last_result);
	last_result= PQexec(connection,query);
	return PQresultStatus(last_result);
}//must be cleared by PQclear

int dbRows(){
	if (last_result)
		return PQntuples(last_result);
	else
		return 0;
}

int dbColumns(){
	if (last_result)
		return PQnfields(last_result);
	else
		return 0;
}

char* dbError(){
	return PQerrorMessage(connection);
}

int dbNumber(char * name){
	return PQfnumber(last_result,name);
}

int dbSize(int row, int col){
	return PQgetlength(last_result, row, col);
}

char *dbValue(int row, int col){
	return PQgetvalue(last_result,row,col);
}
/*
int main(){
	dbConnect(data);
	dbExec("select * from users;");
	printf("%s %d\n",dbValue(1,dbNumber("id")), dbNumber("updated_at"));
	printf("%s %d\n",dbValue(1,dbNumber("email")), dbNumber("updated_at"));
	printf("%s %d\n",dbValue(1,dbNumber("name")), dbNumber("updated_at"));
	dbExec("UPDATE users set name = 'test' where email = 'user@test.test';");
	printf("%s",dbError());
	dbExec("select * from users;");
	printf("%s %d\n",dbValue(1,dbNumber("name")), dbNumber("updated_at"));
	
	dbClose();
	return 0;
}
*/
//UPDATE per­sons set street = 'Nis­sesti­en 67', ci­ty = 'Sand­nes' where lastname = 'Tjes­sem' and firs­tna­me = 'Ja­kob';
//INSERT INTO image (name, raster) VALUES ('beautiful image', 'smth');