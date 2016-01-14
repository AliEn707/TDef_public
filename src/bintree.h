
typedef 
struct bintree{
	void * data;
	struct bintree * next[2];
} bintree;


int bintreeAdd(bintree* root,int key,void* data);

void * bintreeGet(bintree* root, int key);

void * bintreeFetch(bintree* root,int key,int size);

int bintreeDel(bintree* root, int key, void (f)(void*v));

void bintreeErase(bintree * root,void (f)(void*v));

//stops if f returns non zero
void bintreeForEach(bintree * root,int(f)(int k,void *v,void *arg), void* arg);

int bintreeSize(bintree * root);

//returns array of elements created by malloc
void** bintreeToArray(bintree * root);

//create clone of bintree created by malloc
bintree* bintreeClone(bintree * root);
