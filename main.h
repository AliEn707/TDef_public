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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define PLAYER_MAX 200000
#define WORKER_NUM 15


typedef 
struct bintree{
	void * data;
	struct bintree * next[2];
} bintree;

typedef
struct worklist{
	int sock;
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
		char ids[PLAYER_MAX];
	} player;
	
	struct{
		int id;
		int sem;
		pthread_t th;
		int client_num;
		worklist client;
	} worker[WORKER_NUM];
	
	struct{
		int sock;
		int port;//rooms connect port
	} listener;
} config_t;








/////////////
config_t config;
