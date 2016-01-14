#include "headers.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ functions to work with rooms							       ║
║ created by Dennis Yarikov						                       ║
║ jan 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

static bintree rooms;//global rooms container

//for debug
static int fff (int k,void *v,void *arg) {
	room *r=v;
	printf("type %d pl cur %d pl max %d\n", r->type, r->players.current, r->players.max);
	return 0;
}

void printRooms() {
	printf("all rooms:\n");
	roomCheckAll(fff);
	printf("thats all.\n");
}

room * roomGet (int key) { //TODO: make this useful
	return (room *)bintreeGet(&rooms, key);//exist
}

//add room 
int roomAdd(room * r) {
	while(!(r->id = rand())); //have not to get 0
	//TODO: add room to tree of event 
	eventRoomAdd(r->type);
	return bintreeAdd(&rooms, r->id, r);
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
room * roomRem(int key) {
	printRooms();
	room* r=roomGet(key);
	if (r){
		eventRoomDel(r->type);
		bintreeDel(&rooms,key,0);
	}
	return r; //no such element
}

//find rand room by event id
int roomFind(int key) {//ADD: param to search - free slots
	int f=0;
	//TODO: add find event, then find room from it
	int action(int k,void *v,void *arg){
		room *r=v;
		if (r->type==key){
			f=k;
			return k;
		}
		return 0;
	}
	bintreeForEach(&rooms, action, 0);
	return f;
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

int roomLeave(int id){
	room* room=roomGet(id);
	if (room==0)
		return 0;
	room->users--;
//		room->players.current--;
	if (room->users==0)
		free(roomRem(id));
	return 0;
}

room* roomEnter(int id){
	room* room=roomGet(id);
	if (room!=0) {
		room->users++;
//			room->players.current++;
	}
	return room;
}

int roomCheckAll(int(f)(int k,void *v,void *arg)) {
	bintreeForEach(&rooms, f, 0);
	return 0;
}