#include "headers.h"

#define MAX_SERVERS 80

typedef char hostname[25];

typedef
struct {
	int id;
	hostname hostname;
	int port;
	short status;
} server;

static bintree servers; 

static int _servers;

int serversLoad(){
	server * s;
	FILE* file;
	if ((file=fopen("servers.ini","r"))==0){
		perror("fopen servers.ini");
		return 0;
	}
	_servers=0;
	while(!feof(file)){
		int id=_servers+1;
		s=bintreeGet(&servers,id);
		if (s==0){
			s=malloc(sizeof(*s));
			s->id=id;
			bintreeAdd(&servers,id,s);
		}
		fscanf(file,"%s %d",s->hostname,&s->port);
		s->status=1;
		_servers++;
		if (_servers==MAX_SERVERS)
			break;
	}
	
	fclose(file);
/*
	int i;
	t_semop(t_sem.db,&sem[0],1);
		dbSelect("tdef_map_servers");
		_servers=pgRows();
		for(i=0;i<_servers;i++){
			char * value;
			value=pgValue(i,pgNumber("id"));
			int id=atoi(value);
			s=bintreeFetch(&servers,id,sizeof(*s));
			value=pgValue(i,pgNumber("hostname"));
			sprintf(s->hostname,"%s",value);
			value=pgValue(i,pgNumber("port"));
			s->port=atoi(value);
			s->id=id;
		}
		dbClear();
	t_semop(t_sem.db,&sem[1],1);
*/	
	printf("Servers updated\n");
	return _servers;	
}

int serversGetNum(){
	return _servers;
}

int * serversGetSort(){
	static int s_s[MAX_SERVERS+1];
	int j=0;
	int addServ(bintree_key id,void*v,void* arg){
		server * s=v;
		if (s->status>0)
			s_s[j++]=s->id;
		return 0;
	}
	bintreeForEach(&servers,addServ,0);
	*s_s=j;
	return s_s;
}

void serversSetFail(int id){
	server * s=bintreeGet(&servers,id);
	if (s!=0)
		s->status=-1;
}

char * serverGetById(int id){
	server * s=bintreeGet(&servers,id);
	if (s!=0)
		return s->hostname;
	return 0;
}

int serversGetPortById(int id){
	server * s=bintreeGet(&servers,id);
	if (s!=0)
		return s->port;
	return 0;
}

void serversClean(){
	bintreeErase(&servers,free);
}


