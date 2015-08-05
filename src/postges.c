#include <libpq-fe.h>


#define print_error() printf("%s",PQerrorMessage(connection))

static PGconn *connection=0;

static PGresult * last_result=0;
//char data[]="host=localhost\ndbname=wss_devel\nuser=dbuser\npassword=passwd";

int pgConnect(char * cparams){
	connection=PQconnectdb(cparams);
	return PQstatus(connection); 
}

void pgClose(){
	PQfinish(connection);
}

int pgCheck(){
	if (PQstatus(connection)!=0)
		PQreset(connection);
	return PQstatus(connection);
}

void pgClear(){
	if (last_result!=0) 
		PQclear(last_result);
	last_result=0;
}

int pgExec(char * query){
	pgClear();
	last_result= PQexec(connection,query);
	return PQresultStatus(last_result);
}//must be cleared by PQclear


int pgRows(){
	if (last_result)
		return PQntuples(last_result);
	else
		return 0;
}

int pgColumns(){
	if (last_result)
		return PQnfields(last_result);
	else
		return 0;
}

char* pgError(){
	return PQerrorMessage(connection);
}

int pgNumber(char * name){
	return PQfnumber(last_result,name);
}

int pgSize(int row, int col){
	return PQgetlength(last_result, row, col);
}

char *pgValue(int row, int col){
	return PQgetvalue(last_result,row,col);
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