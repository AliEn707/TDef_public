
room * roomGet (int key2);

int roomAdd(room * value);

room * roomRem(int key2);

int roomFind(int key);

int roomFullCheck(room * value);

//void roomFree(room_info* r);

//void * roomNew();

int roomLeave(int id);

room* roomEnter(int id);

int roomCheckAll(int(f)(int k,void *v,void *arg));