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
	int i, ret = 0;
	for(i=0;i<argc;i++){
		if (strcmp(argv[i],"-h")==0){
			printf( 
				#include "help.text.h" 
			);
			exit(0);
		}
		if (strcmp(argv[i],"-debug")==0){
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
		if (strcmp(argv[i],"-w")==0){
			i++;
			config.workers_num = atoi(argv[i]);
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
	}
	return ret;
}

int core(){
	int i;
	char buf[10];
	struct sembuf _sem[2]={{0,-1,0},
						  {0,1,0}};
	
	//clean config
	memcpy(&sem,&_sem,sizeof(sem));
	//set params
	srand(time(0));
	config.run=1;
	
	pgConnectFile("database.cfg");
	dbFillEvents();
	dbFillServers();
	//set tasks for sheduller
	
	//start threads
//	config.sheduller.thread=startSheduller(0);
	config.updater.thread=startUpdater();
	config.serverworker.thread=startServerWorker(0);
	config.watcher.thread=startWatcher(0);
	for(i=0;i<config.workers_num;i++)
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
	scanf("%s",buf);
	printf("exiting\n");
	config.run=0;
	pgClose();
	sleep(5);
	cleanAll();
	return 0;
}

int main(int argc, char* argv[]){
	memset(&config,0,sizeof(config));
	//set defaults
	config.workers_num=WORKER_NUM;
	config.serverworker.port=7000;
	config.player.port=7001;

	parseArgv(argc,argv);
	
	if (config.daemon==0)
		core();
	else
		daemonize(config.pid_file,core);
	return 0;
}