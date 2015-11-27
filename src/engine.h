
int recvData(int sock, void * buf, int size);
int _sendData(int sock, void * buf, int size);

void printLog(const char* format, ...);
#define printf printLog
#define perror(str) printLog("Error: %s\n",str)

int timePassed(struct timeval * t);

void syncTPS(int z,int TPS);

player_info * newPlayer();
void realizePlayer(void *v);

void cleanAll();
/*
void syncTime(struct timeval t, unsigned int time);
*/