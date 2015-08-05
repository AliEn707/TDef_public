
room * roomGet (int key1, int key2);

int roomAdd (int key, void * value);

room * roomRem (int key1, int key2);

int roomFind(int key);

int roomFullCheck(room * value);

room* roomGetByToken(int t);

//void roomFree(room_info* r);

//void * roomNew();

int roomLeave(int type, int id);

room* roomEnter(int type, int id);

int roomCheckAll(int(function)(room *));