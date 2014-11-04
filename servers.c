#include "headers.h"


typedef char hostname[25];

static 
struct {
	hostname hostname;
	int port;
	short status;
} servers[40];

static int _servers;

int serversLoad(){
	FILE* file;
	if ((file=fopen("servers.ini","r"))==0){
		perror("fopen servers.ini");
		return 0;
	}
	memset(servers,0,sizeof(servers));
	_servers=0;
	while(!feof(file)){
		fscanf(file,"%s %d\n",_servers[servers].hostname,&servers[_servers].port);
		servers[_servers].status=1;
		_servers++;
	}
	
	fclose(file);
	printf("Servers updated\n");
	return _servers;	
}

int serversGetNum(){
	return _servers;
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



