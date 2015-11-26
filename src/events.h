

typedef
struct {
	int id;
	int timestamp; //change on $rooms change 
	int $rooms;
	char name[30];
	char map[20];
	char single;
	int bitmask;
	bintree dependences;
} event;


event * eventAdd(int id);

event * eventGet(int id);

int eventDel(int id);

int eventRoomAdd(int id);

int eventRoomDel(int id);

int eventForEach(void * p, int(f)(void*,event*));

int eventErase();