
typedef
struct {
	int id;
	int timestamp;
	int $rooms;
} event;


event * eventAdd(int id);

int eventDel(int id);

int eventRoomAdd(int id);

int eventRoomDel(int id);

int eventForEach(void * p, int(f)(void*,event*));
