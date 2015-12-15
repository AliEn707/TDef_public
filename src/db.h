#include "postgres.h"
typedef int dbUpdate_t;

int dbConnect(char* config);
void dbClear(dbQuery_t id);

player_info * dbAuth(worklist * client);

int dbGetPlayer(player_info * pl, char * name, int t);

int dbFillServers();

int dbFillEvents();

char* dbTime(time_t t);
time_t dbRawTime(char* t);

#define dbError() pgError()

//selects
dbQuery_t dbSelect(char* table);
dbQuery_t dbSelectWhereUni(char* table, char* fields, char* cmp);
dbQuery_t dbSelectWhere(char* table, char* field, char* cmp, char* value);
dbQuery_t dbSelectWhereNewer(char* table, char* field, char* cmp, char* value, time_t timestamp);
dbQuery_t dbSelectField(char* table, char* field);
dbQuery_t dbSelectFieldWhere(char* table, char* sel, char* field, char* cmp, char* value);
dbQuery_t dbSelectFieldWhereNewer(char* table, char* sel, char* field, char* cmp, char* value, time_t timestamp);
dbQuery_t dbSelectNewer(char* table, time_t timestamp);
dbQuery_t dbSelectFieldNewer(char* table,char* field, time_t timestamp);

dbUpdate_t dbUpdateStart(char* table);
void dbUpdateValue(dbUpdate_t, char *field, char *value);
void dbUpdateEnd(dbUpdate_t, char *cmp, int touch);

//one line insert
void dbInsert(char* table, char * fields, char * values);
//more lines insert
dbUpdate_t dbInsertStart(char* table);
void dbInsertValue(dbUpdate_t, char *field, char *value);
void dbInsertEnd(dbUpdate_t);

//add log
void dbLog(int player_id, char *action, int object_id, char *object_type, int value, char* other);