//#include "headers.h"


#include <map>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
/*
╔══════════════════════════════════════════════════════════════╗
║ 		functions for binary tree with data 			                       ║
║ created by Dennis Yarikov						                       ║
║ edited by Yaroslav Zotov						                       ║
║ nov 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/
extern "C" {

#include "events.h"
}

event _event[1]; //change to map of events
using namespace std;

map <int, event * > events;


extern "C" {
	#include <sys/types.h>
	#include <time.h>
	#include <string.h>
	
	event * eventAdd(int _key){
		//add some stuff
		event* e;
		if (events.find(_key) == events.end()) {
			if ((e=(event*)malloc(sizeof(event)))==0)
				return 0;
			memset(e,0,sizeof(event));
			e->id=_key;
			e->timestamp=time(0);
			events[_key] = e;
		}
		return events[_key];
	}
	
	event * eventGet(int _key){
		//add some stuff
		if (events.find(_key) != events.end())
			return events[_key];
		return 0;
	}
	
	int eventDel(int id){
		map <int, event * >::iterator it = events.find(id);
		if (it != events.end()) {
			free(it->second);
			events.erase(it);
			return 0;
		}
		return 1;
	}
	
	int eventRoomAdd(int id){
		map <int, event * >::iterator it = events.find(id);
		if (it != events.end()) {
			events[id]->timestamp=time(0);
			events[id]->$rooms++;
			return 0;
		}
		return 1;
	}
	
	int eventRoomDel(int id){
		map <int, event * >::iterator it = events.find(id);
		if (it != events.end()) {		
			events[id]->timestamp=time(0);
			events[id]->$rooms--;		
			return 0;
		}
		return 1;
	}
	
	int eventForEach(void * p, int(f)(void*,event*)){
		//add some stuff
		typedef map <int, event *>::iterator it_type;
		for(it_type iterator = events.begin(); iterator != events.end(); iterator++) {
//			std::cout << iterator->second << std::endl;
			f(p,iterator->second);
		}
		
		return 0;
	}
	
}
