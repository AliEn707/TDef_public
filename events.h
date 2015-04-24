
typedef
struct {
	int id;
	int timestamp;
	int $rooms;
	char name[20];
	char map[20];
	int bitmask;
} event;


event * eventAdd(int id);

event * eventGet(int id);

int eventDel(int id);

int eventRoomAdd(int id);

int eventRoomDel(int id);

int eventForEach(void * p, int(f)(void*,event*));
