
int recvData(int sock, void * buf, int size);
int _sendData(int sock, void * buf, int size);

void printLog(const char* format, ...);
#define printf(format, ...) printLog(format, ## __VA_ARGS__)
#define perror(str) printLog("Error: %s\n",str)

int timePassed(struct timeval * t);

void syncTPS(int z,int TPS);

player_info * newPlayer();
void realizePlayer(void *v);

void cleanAll();
/*
void syncTime(struct timeval t, unsigned int time);
*/
int wrongByteOrder();

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#define biteSwap(a) ({ typeof(a) out=a;\
	if (sizeof(a)==2){\
		short val=*((short*)&(a));\
		val=(val << 8) | ((val >> 8) & 0xFF);\
		out=*((typeof(a)*)&val);\
	}else if (sizeof(a)==4){\
		int val=*((int*)&(a));\
		val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );\
		val = (val << 16) | ((val >> 16) & 0xFFFF);\
		out=*((typeof(a)*)&val);\
	} else if (sizeof(a) == 8){\
		long long val=*((long long*)&(a));\
		val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );\
		val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );\
		val = (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);\
		out=*((typeof(a)*)&val);\
	}\
out;})
//TODO: change to faster version

#else

#define biteSwap(a) (a)

#endif

//example of usage #define func(a, c) ({typeof(a) b=biteSwap(a); c(&b);})
//#define func(a) ({typeof(a) b=biteSwap(a); original_func(&b);})