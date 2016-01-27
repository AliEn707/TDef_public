#include "headers.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ functions to work with rooms							       ║
║ created by Dennis Yarikov						                       ║
║ jan 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

static bintree events;//global rooms container


event * eventAdd(int _key){
	//add some stuff
	event* e;
	if ((e=bintreeGet(&events, _key))==0){
		if ((e=(event*)malloc(sizeof(event)))==0)
			return 0;
		memset(e,0,sizeof(event));
		e->id=_key;
		e->timestamp=time(0);
		bintreeAdd(&events, _key, e);
	}
	return e;
}

event * eventGet(int _key){
	//add some stuff
	return bintreeGet(&events, _key);
}

int eventDel(int _key){
	return bintreeDel(&events, _key, free);
}

int eventRoomAdd(int id){
	event* e=eventGet(id);
	if (e!=0) {
		e->timestamp=time(0);
		e->$rooms++;
		return 0;
	}
	return 1;
}

int eventRoomDel(int id){
	event* e=eventGet(id);
	if (e!=0) {	
		e->timestamp=time(0);
		e->$rooms--;		
		return 0;
	}
	return 1;
}

int eventForEach(void * p, int(f)(bintree_key k,void *v,void *arg)){
	bintreeForEach(&events, f, p);
	return 0;
}

int eventErase(){
	bintreeErase(&events, free);
	return 0;
}