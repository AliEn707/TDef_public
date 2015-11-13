#include "main.h"

#ifndef t_sem
struct sembuf {
	short sem_num;
	short sem_op;
	short flags;
};
#endif

struct t_sem_t {
	pthread_mutex_t mutex;
	short *val;
};

//typedef int t_sem_t;
typedef struct t_sem_t *t_sem_t;
//#define t_sem_t int
/*
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
*/

struct t_sem_struct{
	t_sem_t watcher;
	t_sem_t sheduller;
	t_sem_t updater;
	t_sem_t serverworker;
	t_sem_t room;
	t_sem_t player;
	t_sem_t events;
	t_sem_t worker[WORKER_NUM];
	t_sem_t db;
} t_sem;

struct sembuf sem[2];

#ifndef IPC_PRIVATE

#define IPC_PRIVATE 0
#define IPC_RMID 4096
#define IPC_CREAT  0x0200

#endif
#define t_semop semop
#define t_semctl semctl
#define t_semget semget



#undef t_semop
#undef t_semctl
#undef t_semget

int t_semop(t_sem_t semid, struct sembuf *sops, unsigned nsops);

t_sem_t t_semget(key_t key, int nsems, int semflg);

int t_semctl(t_sem_t semid, int semnum, int cmd);
