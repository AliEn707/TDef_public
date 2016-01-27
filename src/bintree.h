
typedef 
struct bintree{
	void * data;
	struct bintree * next[2];
} bintree;

typedef long long bintree_key;

bintree_key bintreeAdd(bintree* root, bintree_key key,void* data);

void * bintreeGet(bintree* root, bintree_key key);

void * bintreeFetch(bintree* root, bintree_key key,int size);

int bintreeDel(bintree* root, bintree_key key, void (f)(void*v));

void bintreeErase(bintree * root,void (f)(void*v));

//stops if f returns non zero
void bintreeForEach(bintree * root,int(f)(bintree_key k,void *v,void *arg), void* arg);

int bintreeSize(bintree * root);

//returns array of elements created by malloc
void** bintreeToArray(bintree * root);

//create clone of bintree created by malloc
bintree* bintreeClone(bintree * root);
