
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


#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#define biteSwap(a) ({ typeof(a) val=a;\
	if (sizeof(a)==2){\
		val=(val << 8) | ((val >> 8) & 0xFF);\
	}else if (sizeof(a)==4){\
		val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );\
		val = (val << 16) | ((val >> 16) & 0xFFFF);\
	} else if (sizeof(a) == 8){\
		val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );\
		val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );\
		val = (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);\
	}\
val;})


#else

#define biteSwap(a) a

#endif

//example of usage #define func(a, c) ({typeof(a) b=biteSwap(a); c(&b);})
//#define func(a) ({typeof(a) b=biteSwap(a); original_func(&b);})