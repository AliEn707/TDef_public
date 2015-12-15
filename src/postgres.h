#ifndef POSTGRES_HEADER
#define POSTGRES_HEADER

#include <libpq-fe.h>

#define DB_CONNECTIONS 50

typedef int dbQuery_t;

typedef 
struct {
	short used;
	PGresult * query;
} dbConnection;


int pgConnect(char * cparams);

void pgClose();
//chack connection and reconnect if needed
int pgCheck();

dbQuery_t pgExec(char * query);
void pgClear(dbQuery_t id);

int pgRows(dbQuery_t id);
int pgColumns(dbQuery_t id);

//get last error
char* pgError();
void pgErrorPrint();

//number of column by name
int pgNumber(dbQuery_t id, char * name);

//size of value
int pgSize(dbQuery_t id, int row, int col);

char *pgValue(dbQuery_t id, int row, int col);

#endif