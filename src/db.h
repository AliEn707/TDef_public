
int dbConnect(char* config);
void dbClear();

player_info * dbAuth(worklist * client);

int dbGetPlayer(player_info * pl, char * name, int t);

int dbFillServers();

int dbFillEvents();

char* dbTime(time_t t);
time_t dbRawTime(char* t);

#define dbError() pgError()

//selects
int dbSelect(char* table);
int dbSelectWhereUni(char* table, char* fields, char* cmp);
int dbSelectWhere(char* table, char* field, char* cmp, char* value);
int dbSelectWhereNewer(char* table, char* field, char* cmp, char* value, time_t timestamp);
int dbSelectField(char* table, char* field);
int dbSelectFieldWhere(char* table, char* sel, char* field, char* cmp, char* value);
int dbSelectFieldWhereNewer(char* table, char* sel, char* field, char* cmp, char* value, time_t timestamp);
int dbSelectNewer(char* table, time_t timestamp);
int dbSelectFieldNewer(char* table,char* field, time_t timestamp);

int dbUpdateStart(char* table);
int dbUpdateValue(char *field, char *value);
int dbUpdateEnd(char *cmp, int touch);

//one line insert
int dbInsert(char* table, char * fields, char * values);
//more lines insert
int dbInsertStart(char* table);
int dbInsertValue(char *field, char *value);
int dbInsertEnd();

//add log
int dbLog(int player_id, char *action, int object_id, char *object_type, int value, char* other);