
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

void bintreeForEach(bintree * root,void* arg,void(f)(void* arg,int k,void*v));

int bintreeSize(bintree * root);

void* bintreeToArray(bintree * root);
bintree* bintreeClone(bintree * root);
