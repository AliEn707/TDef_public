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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define PLAYER_MAX 200000
#define WORKER_NUM 15

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
	int sock;
	int id;
	void * data;
	struct worklist * next;
} worklist;

typedef
struct {
	int id;
	
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




/////////////
config_t config;
struct sembuf sem[2];


