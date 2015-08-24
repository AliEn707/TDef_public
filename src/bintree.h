
int bintreeAdd(bintree* root,int key,void* data);

void * bintreeGet(bintree* root, int key);

int bintreeDel(bintree* root, int key, void (f)(void*v));

void bintreeErase(bintree * root,void (f)(void*v));
