
int dbConnect(char* config);

player_info * dbAuth(worklist * client);

int dbGetPlayer(player_info * pl, char * name);

int dbFillServers();

int dbFillEvents();

char* dbTime(int t);

#define dbError() pgError()

//selects
int dbSelect(char* table);
int dbSelectWhere(char* table, char* field, char* cmp, char* value);
int dbSelectWhereNewer(char* table, char* field, char* cmp, char* value, int timestamp);
int dbSelectField(char* table, char* field);
int dbSelectNewer(char* table, int timestamp);
int dbSelectFieldNewer(char* table,char* field, int timestamp);