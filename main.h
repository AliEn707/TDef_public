#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define PLAYER_MAX 200000
#define WORKER_NUM 1


#define PLAYER_CONNECTED 2
#define PLAYER_IN_LOBBY 2
#define PLAYER_IN_GAME 3

//room statuses
#define ROOM_PREPARE 1 //wait for proceed
#define ROOM_RUN 2	//allready played
#define ROOM_FAIL 0	//cant create room
#define ROOM_ERROR 3	//need to try create

//player bitmasks
#define BM_PLAYER_CONNECTED 1
#define BM_PLAYER_STATUS 2
#define BM_PLAYER_TIMESTAMP 4

//out message types
#define MESSAGE_DATA_CHANGE 1

//in message types
#define MESSAGE_LOBBY 76//'L'

//room actions
#define MESSAGE_CREATE_ROOM 99 //'c'
#define MESSAGE_FAST_ROOM 2
#define MESSAGE_FIND_ROOM 4

//messages from servers
#define MESSAGE_ROOM_STATUS 1
#define MESSAGE_ROOM_RESULT 2

//connection
#define CONNECTED 1
#define FAIL 0


typedef 
struct {
	int users;
	int stat; //0 free, -1 need to del, 1 used
	struct {
		short max;
		short current;
	} players;
	int port;
	int server; //FIX ME
	int timestamp;
	int token;
	int bitmask;
	short status;
	float timer;
	
	int id;
	int type;
} room;

typedef
struct {
	long id;
	char buf[100];
} message;

typedef 
struct bintree{
	void * data;
	struct bintree * next[2];
} bintree;

typedef
struct worklist{
	int sock; //client socket
	int id;  //client id
	void * data;
	struct worklist * next;
} worklist;

typedef
struct {
	//not base
	short conn;  //connection status
	short status;  //player status (location, ...)
	int bitmask;  
	int timestamp;  //need for sync checks
	struct {
		int token;
		//int port;  use id->info->port
		int type; //room that player attach
		int id; //room that player attach
	} room;
	//base
	int id;  //player id  from base ??
	
} player_info;

typedef 
struct {
	short run;
	
	struct{
		int sock;
		int port;//players connect port
		bintree tree;
		int sem;
		char ids[PLAYER_MAX];
	} player;
	
	struct{
		int id;
		int client_num;
		int sem;
		worklist client;
		pthread_t thread;
	} worker[WORKER_NUM];
	
	struct{
		pthread_t thread;
	} listener;
	
	struct{
		int msg;
		int sem;
		worklist task;
		pthread_t thread;
	} sheduller;
	
	struct{
		int client_num;
		int sem;
		worklist client;
		pthread_t thread;
	} watcher;
	
	struct{
		int id;
		int client_num;
		int sock;//socket for servers
		int port;//rooms connect port
		int sem;
		worklist client;
		pthread_t thread;
	} serverworker;
	
} config_t;



#define getMsg(msg) msgrcv(config.sheduller.msg,&msg,sizeof(msg),0,MSG_NOERROR|IPC_NOWAIT)
#define sendMsg(msg) msg.id=1;msgsnd(config.sheduller.msg,&msg,sizeof(msg),0);

#define sendData(sock,x,y) if(send(sock,x,y,MSG_NOSIGNAL)<=0) return -1


#define setMask(mask, p) (mask|=p)
#define checkMask(mask, p) (mask&p)

/////////////
config_t config;
struct sembuf sem[2];


