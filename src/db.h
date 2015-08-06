
int dbConnect(char* config);

player_info * dbAuth(worklist * client);

int dbGetNpcTypes(int t);
int dbGetPlayer(player_info * pl, char * name);

int dbFillServers();

int dbFillEvents();

char* dbTime(int t);

#define dbError() pgError()