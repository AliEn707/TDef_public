
int recvData(int sock, void * buf, int size);
int _sendData(int sock, void * buf, int size);

void printLog(const char* format, ...);
#define printf printLog

int timePassed(struct timeval * t);

void syncTPS(int z,int TPS);

int newPlayerId();

int delPlayerId(int id);

void cleanAll();
/*
void syncTime(struct timeval t, unsigned int time);
*/