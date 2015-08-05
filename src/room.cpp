#include <map>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>



/*
╔══════════════════════════════════════════════════════════════╗
║ contaners for storing room data					                       ║
║ 										                       ║
║ created by Yaroslav Zotov						                       ║
║ edited by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


using namespace std;

map <int, map <int, void*> > rooms_map;
typedef map <int, void*>::iterator it;
typedef map <int, map <int, void*> >::iterator it_big;


extern "C" {
/*
typedef 
struct {
	int users;
	struct {
		short max;
		short current;
	} players;
	int port;
	int server; //FIX ME
	int timestamp;
	int token;
	int bitmask;
	short status;
	float timer;
} room;*/

	#include "main.h"
	#include "room.h"
	#include "events.h"

//for debug
	int fff (room *r) {
		cout <<"type "<< r->type << 
			" token " << r->token<<
			" pl cur "<< r->players.current << 
			" pl max " << r->players.max << endl;
		return 0;
	}
	
	void printRooms() {
		printf("all rooms:\n");
		roomCheckAll(fff);
		printf("thats all.\n");
	}
	
	room * roomGet (int key1, int key2) { //TODO: make this useful
		if (rooms_map.find(key1) == rooms_map.end() || rooms_map[key1].find(key2) ==rooms_map[key1].end()) {
			return NULL;//no such element
		}
		return (room *)rooms_map[key1][key2];//exist
	}
	//add room to map
	int roomAdd (int key, void * value) {
		for (it i = rooms_map[key].begin(); i != rooms_map[key].end(); i++)
			if (i->second == value)
				return 0; //exist
		int randomIndex = rand() + 1;
		while (1) {
			if (rooms_map[key].find(randomIndex) != rooms_map[key].end()) { //key already exists
				randomIndex = rand() + 1; //generate new random key
				continue;
			}
			rooms_map[key][randomIndex] = value;
			eventRoomAdd(key);
			return randomIndex;
		}
	}
	//del room_info from map, not doing free
/*
	room * roomRem (int key1, int key2) {
		for (it i = rooms_map[key1].begin(); i != rooms_map[key1].end();) {
			if (i->first == key2) {
				room * tmp = (room *) i->second;
				rooms_map[key1].erase(i++);
				eventRoomDel(key1);
				return tmp; //removed
			}
			else
				++i;
		}
		return NULL; //no such element
	}
*/
	room * roomRem (int key1, int key2) {
		printRooms();
		for (it i = rooms_map[key1].begin(); i != rooms_map[key1].end(); i++)
			if (i->first == key2) {
				room * tmp = (room *) i->second;
				rooms_map[key1].erase(i);
				eventRoomDel(key1);
				printf("room %d on %d removed\n",key2,key1);
				printRooms();
				return tmp; //removed
			}
//		perror("room not rem");
		return NULL; //no such element
	}
	//find rand room by key
	int roomFind(int key) {//ADD: param to search - free slots
		if (rooms_map[key].size()== 0)
			return 0;
		vector<int> temp;
		for (it i = rooms_map[key].begin(); i != rooms_map[key].end(); i++)
			if (roomFullCheck((room*)i->second))
				temp.push_back(i->first);
		if (temp.size() == 0)
			return 0;
		return temp[rand()%temp.size()];
	}
	
			
	//check room for to be full
	int roomFullCheck(room * value) {
		return value->players.current < value->players.max;
	}
	
	/*//get room_info for free room
	void * roomNew(){
		int i;
		for(i=0;i<PLAYER_MAX;i++)
			if (rooms[i].stat==0){
				if ((rooms[i].info=(room*)malloc(sizeof(room)))==0)
					perror("malloc proceedMessage");
				memset(rooms[i].info,0,sizeof(room));
				return &rooms[i];
			}
		return 0;
	}
	
	//free info of room from room_info, if no one user use it
	void roomFree(room_info* r){
		if (r->users>0)
			return;
		free(r->info);
		memset(r,0,sizeof(room_info));
	}*/
	
	room* roomGetByToken(int t){//TODO: FIX
		cout<<"token"<< t << "\n";
		for (it_big i = rooms_map.begin(); i != rooms_map.end(); i++)
			for (it j = rooms_map[i->first].begin(); j != rooms_map[i->first].end(); j++)
				if (((room*)j->second)->token == t)
					return (room*)j->second;
//		perror("room not find by token");
		return NULL;
	}
	
	int roomLeave(int type, int id){
		room* room=roomGet(type,id);
		if (room==0)
			return 0;
		room->users--;
//		room->players.current--;
		if (room->users==0)
			free(roomRem(type,id));
		return 0;
	}
	
	room* roomEnter(int type, int id){
		room* room=roomGet(type,id);
		if (room!=0) {
			room->users++;
//			room->players.current++;
		}
		return room;
	}
	
	int roomCheckAll(int($f)(room * r)) {
		vector <void*> v_v;
		for (it_big i = rooms_map.begin(); i != rooms_map.end(); i++){
			for (it j = i->second.begin(); j != i->second.end(); j++)
				v_v.push_back(j->second);
		}
		unsigned int i;
		for (i=0;i<v_v.size();i++)
			$f((room *)v_v[i]);
		return 0;
	}
	
}
/*


int main() {
	//srand(NULL);
	room rooms[100];
	for (int i = 0 ; i < 100; i++) {
		rooms[i].users = rand()%32;
		rooms[i].players.current = rand()%32;
		rooms[i].players.max = rand()%32;
		//std::cout << rooms[i].info << " " << rooms[i].users << "\n";
	}
	//cout << roomAdd(100500, rooms + 15) << endl;
	rooms[16].players.max = 5;
	rooms[16].players.current = 5;
	cout << roomAdd(100500, rooms + 16) << endl;
	//cout << roomAdd(100500, rooms + 28) << endl;
	cout << roomAdd(-10, rooms) << endl;
	cout << roomAdd(1, rooms + 55) << endl;
	//roomRem(100500, rooms + 15);
	printRooms();
	cout << endl << endl;
	roomCheckAll(fff);
	cout << roomFind(100500) << endl;
	return 0;
}
*/