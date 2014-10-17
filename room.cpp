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

map <int, vector<void*> > rooms_map;



extern "C" {

	#include "main.h"
	#include "room.h"
	
	
	int roomGet (int key) { //TODO: make this useful
		if (rooms_map.find(key) == rooms_map.end()) {
			return 1;//no such element
		}
		return 0;//exist
	}
	//add room_info to map
	int roomAdd (int key, void * value) {
		int length = rooms_map[key].size();
		for (int i = 0; i < length; i++)
			if (rooms_map[key][i] == value)
				return 1; //exist
		rooms_map[key].push_back(value);
		return 0;
	}
	//del romm_info from map, not doing free
	int roomRem (int key, void * value) {
		int length = rooms_map[key].size();
		for (int i = 0; i < length; i++)
			if (rooms_map[key][i] == value) {
				rooms_map[key].erase(rooms_map[key].begin() + i);
				return 0; //removed
			}
		return 1; //no such element
	}
	//find rand roon by key
	void * roomFind(int key) {
		int length = rooms_map[key].size();
		if (length == 0)
			return NULL;
		vector<void*> temp;
		for (int i = 0; i < length; i++)
//			if (roomFullCheck((room*)(*((void**)rooms_map[key][i]))))
			if (roomFullCheck(((room_info*)rooms_map[key][i])->info))
				temp.push_back(rooms_map[key][i]);
		cout << temp.size() << endl;
		return temp[rand()%temp.size()];
	}
/*
	void printRooms() {
		for (map <int, vector<void*> >::iterator i = rooms_map.begin(); i != rooms_map.end(); i++) {
			int key = i->first;
			cout << key << ":\n";
			for (int j = 0; j < rooms_map[key].size(); j++)
				cout << rooms_map[key][j] << endl;
			cout << endl;
		}
	}
*/	
	//check room for to be full
	int roomFullCheck(room * value) {
		cout << value->players.current << " " << value->players.max << endl << " " <<  endl;
		return value->players.current < value->players.max;
	}
	
	//get room_info for free room
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
	
	//free info of room from room_info, if noone user use it
	void roomFree(room_info* r){
		if (r->users>0)
			return;
		free(r->info);
		memset(r,0,sizeof(room_info));
	}
	
	room_info * roomGetByToken(int t){
		int i;
		for(i=0;i<PLAYER_MAX;i++)
			if(rooms[i].stat>0)
				if (rooms[i].info->token==t)
					return &rooms[i];
		return 0;
	}
}

/*
int main() {
	srand(time(0));
	room array[PLAYER_MAX];
	for (int i = 0 ; i < PLAYER_MAX; i++) {
		rooms[i].users = rand()%32;
		rooms[i].info = &array[i];
		//std::cout << rooms[i].info << " " << rooms[i].users << "\n";
	}
	roomAdd(100500, rooms + 15);
	array[15].players.max = 5;
	array[15].players.current = 5;
	roomAdd(100500, rooms + 15);
	roomAdd(100500, rooms + 28);
	roomAdd(-10, rooms);
	roomAdd(1, rooms + 55);
	//roomRem(100500, rooms + 15);
	printRooms();
	cout << roomFind(100500) << endl;
	return 0;
}
*/
