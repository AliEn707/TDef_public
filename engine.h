
int recvData(int sock, void * buf, int size);

int timePassed(struct timeval * t);

void syncTPS(int z,int TPS);

int newPlayerId();

int delPlayerId(int id);

void cleanAll();
/*
void syncTime(struct timeval t, unsigned int time);
*/