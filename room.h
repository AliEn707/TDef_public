
int roomGet (int key);

int roomAdd (int key, void * value);

int roomRem (int key, void * value);

void * roomFind(int key);

int roomFullCheck(room * value);

room_info * roomGetByToken(int t);

void roomFree(room_info* r);

void * roomNew();