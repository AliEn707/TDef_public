#include "headers.h"


typedef char hostname[25];

static 
struct {
	hostname hostname;
	int port;
	short status;
} servers[40];

static int $servers;

int serversLoad(){
	FILE* file;
	if ((file=fopen("servers.ini","r"))==0){
		perror("fopen servers.ini");
		return 0;
	}
	memset(servers,0,sizeof(servers));
	$servers=0;
	while(!feof(file)){
		fscanf(file,"%s %d\n",$servers[servers].hostname,&servers[$servers].port);
		servers[$servers].status=1;
		$servers++;
	}
	
	fclose(file);
	printf("Servers updated\n");
	return $servers;	
}

int serversGetNum(){
	return $servers;
}

void serversSetFail(int id){
	servers[id].status=-1;
}

char * serverGetById(int id){
	if (servers[id].status>0)
		return servers[id].hostname;
	return 0;
}

int serversGetPortById(int id){
	return servers[id].port;
}



