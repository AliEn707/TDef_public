#include <signal.h>

#include "headers.h"
 
/*
╔══════════════════════════════════════════════════════════════╗
║ thread listen sockets 							                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

int daemonize(char* log_file, int (core)());
/*
You can often see in this code $_$. 
$_$ is the Scrooge_McDuck. 
He helps us in this not simple work. 
*/
//Time.now.to_i in ruby to get time() val


int parseArgv(int argc,char * argv[]){
	#include "help.text.h" 
	int i, ret = 0;
	for(i=0;i<argc;i++){
		if (strcmp(argv[i],"-h")==0){
			config.debug = 1;
			printf( "%s", text);
			exit(0);
		}
		if (strcmp(argv[i],"-debug")==0 || strcmp(argv[i],"-v")==0){
			config.debug = 1;
			continue;
		}
		if (strcmp(argv[i],"-d")==0){
			config.daemon = 1;
			continue;
		}
		if (strcmp(argv[i],"-p")==0){
			i++;
			config.pid_file = argv[i];
			continue;
		}
		if (strcmp(argv[i],"-c")==0){
			i++;
			config.player.port = atoi(argv[i]);
			continue;
		}
		if (strcmp(argv[i],"-s")==0){
			i++;
			config.serverworker.port = atoi(argv[i]);
			continue;
		}
		if (strcmp(argv[i],"-l")==0){
			i++;
			config.log_file = argv[i];
			continue;
		}
		if (strcmp(argv[i],"-i")==0){
			config.debug = 1;
			printf("Built at %s %s\n", __DATE__,__TIME__);
			exit(0);
		}
	}
	return ret;
}

//action fired on signals
void exit_sigaction(int signal, siginfo_t *si, void *arg){
	config.run=0;
//	usleep(100000);
//	exit(0);
}

int core(){
	int i;
	int TPS=3;
	char buf[10];
	struct sembuf _sem[2]={{0,-1,0},
						  {0,1,0}};
	struct timeval tv={0,0};
	
	struct sigaction sa;

	memset(&sa, 0, sizeof(sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = exit_sigaction;
	sa.sa_flags   = SA_SIGINFO;

//	sigaction(SIGSEGV, &sa, NULL);	
	sigaction(SIGINT, &sa, NULL);	
	sigaction(SIGTERM, &sa, NULL);	
	
	timePassed(&tv);
	//copy semaphores
	memcpy(&sem,&_sem,sizeof(sem));
	//set params
	srand(time(0));
	config.run=1;
	
	dbConnect("database.cfg");
	dbFillEvents();
	dbFillServers();
	//set tasks for sheduller
	
	
	//set semaphores
	if((t_sem.log=t_semget(IPC_PRIVATE, 1, 0755 | IPC_CREAT))==0)
		perror("(main) semget log");
	t_semop(t_sem.log,&sem[1],1);
	
	//start threads
//	config.sheduller.thread=startSheduller(0);
	config.updater.thread=startUpdater();
	config.serverworker.thread=startServerWorker(0);
	config.watcher.thread=startWatcher(0);
	for(i=0;i<WORKER_NUM;i++)
		config.worker[i].thread=startWorker(i);
	config.listener.thread=startListener();
	//start console for external commands
	
	//some garbage					  
//	for(i=0;i<20;i++){
//		sprintf(msg.buf,"hello");
//		sendMsg(msg);
//		usleep((rand()%9)*100000);
//	}
	printf("done\n");
	while (config.run){
		
		if (scanf("%s",buf)!=0)  //replace by something normal
			config.run=0;
		
		syncTPS(timePassed(&tv),TPS);
	}
	printf("exiting\n");
	
	pgClose();
	sleep(5);
	cleanAll();
	return 0;
}

int main(int argc, char* argv[]){
	memset(&config,0,sizeof(config));
	memset(&t_sem,0,sizeof(t_sem));
	//set defaults
	config.serverworker.port=7000;
	config.player.port=7001;

	config.debug = 1; //always debug for devel
	
	parseArgv(argc,argv);
	
	if (wrongByteOrder())
		return 0;
	
	if (config.daemon==0)
		core();
	else
		daemonize(config.pid_file,core);
	return 0;
}