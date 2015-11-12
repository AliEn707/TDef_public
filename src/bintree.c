#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "main.h"



/*
╔══════════════════════════════════════════════════════════════╗
║ 		functions for binary tree with data 			                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

static int reverseInt(int x){
	int o;
	while (x){
		o=(o<<1)+(x&1);
		x>>=1;
	}
	return o;
}

int bintreeAdd(bintree* root,int key,void* data){
	bintree* tmp=root;
	int next;
//	printDebug("add key %d\n",key);
	while(key>0){
		next=(int)(key&1);
		if(tmp->next[next]==0){
			if((tmp->next[next]=malloc(sizeof(bintree)))==0){
				perror("malloc bintreeAdd");
				return 0;
			}
			memset(tmp->next[next],0,sizeof(bintree));
		}
		tmp=tmp->next[next];
		key>>=1;
	}
	if (tmp->data!=0)
		return 0;
	tmp->data=data;
	return 1;
}

void * bintreeGet(bintree* root, int key){
	bintree* tmp=root;
	int next;
	while(key>0){
		next=(int)(key&1);
		if(tmp->next[next]==0)
			return 0;
		tmp=tmp->next[next];
		key>>=1;
	}
	return tmp->data;
}

//get or add object size of "size"
void * bintreeFetch(bintree* root,int key,int size){
	void * a=bintreeGet(root,key);
	if (a==0){
		a=malloc(size);
		bintreeAdd(root,key,a);
	}
	return a;
}

int _bintreeDel(bintree* root, int key, void (f)(void*v)){
	int get;
	int next;
	if (root==0)
		return 0;
	if (key==0){
		if (f!=0)
			f(root->data);
//		free(root->data); //may be need
		root->data=0;
		if (root->next[0]==0 && root->next[1]==0){
			free(root);
//			printDebug("free\n");
			return 1;
		}
		return 0;
	}
	next=key&1;
	get=_bintreeDel(root->next[next],key>>1,f);
//	free(root->next[next]);
	if (get!=0){
		root->next[next]=0;
		if (root->next[(next+1)%2]==0){
//			printDebug("%d %d\n",next,(next+1)%2);
			free(root);
			return 1;
		}
	}
	return 0;
}

int bintreeDel(bintree* root, int key, void (f)(void*v)){
	int get;
	int next=key&1;
	get=_bintreeDel(root->next[next],key>>1,f);
	if (get!=0)
		root->next[next]=0;
	return get;
}

/*
int bintreeDel(bintree* root, int key){
	bintree* tmp=root;
	int next;
	while(key>1){
		next=(int)(key&1);
		if(tmp->next[next]==0)
			return 0;
		tmp=tmp->next[next];
		key>>=1;
	}
	free(tmp->next[next]);
	tmp->next[next]=0;
	return 1;
}
*/
void bintreeErase(bintree * root,void (f)(void*v)){
	if (root==0)
		return;
//	printDebug("y\n");
	bintreeErase(root->next[0],f);
	bintreeErase(root->next[1],f);
	if (f!=0)
		f(root->data);
	free(root->next[0]);
	root->next[0]=0;
	free(root->next[1]);
	root->next[1]=0;
}

static void _bintreeForEach(bintree * root, void(f)(int k, void *v, void *arg), void* arg, int key){
	if (root==0)
		return;
	int i;
	for(i=0;i<2;i++)
		_bintreeForEach(root->next[i],f,arg,(key<<1)+i);
	if (root->data!=0)
		f(reverseInt(key)>>1,root->data, arg);
}

void bintreeForEach(bintree * root, void(f)(int k, void *v, void *arg), void *arg){
	if (root==0)
		return;
	_bintreeForEach(root,f,arg,1);
}

int bintreeSize(bintree * root){
	int i=0;
	void add(int k, void*v, void* arg){
		i++;
	}
	bintreeForEach(root,add, 0);
	return i;
}

//get all values as array
void** bintreeToArray(bintree * root){
	void** o;
	int i=bintreeSize(root);
	void add(int k, void*v, void* arg){
		o[i]=v;
		i++;
	}
	if ((o=malloc(sizeof(*o)*i))==0)
		return 0;
	i=0;
	bintreeForEach(root, add, 0);
	return o;
}

//get clone of bintree
bintree* bintreeClone(bintree * root){
	bintree* o;
	void add(int k, void*v, void* arg){
		bintreeAdd(o,k,v);
	}
	if ((o=malloc(sizeof(*o)))==0)
		return 0;
	memset(o,0,sizeof(*o));
	bintreeForEach(root,add,0);
	return o;
}

/*
void show(int k, void* v, void* a){
	printf("%d %d\n",k,v);
}

int main(){
	bintree r;
	bintree * z;
	memset(&r,0,sizeof(r));
	
	printf("%u\n",reverseInt(11));
	
	bintreeAdd(&r,3,(void*)100);
	bintreeAdd(&r,6,(void*)143);
	bintreeAdd(&r,7,(void*)132);
	printf("%d\n",bintreeGet(&r,3));
	bintreeForEach(&r,show,0);
	printf("\t %d\n", bintreeSize(&r));
	free(bintreeToArray(&r));

	bintreeDel(&r,3,0);
	printf("\t %d\n", bintreeSize(&r));
//	bintreeDel(&r,3);
	printf("%d\n",bintreeGet(&r,3));
	printf("%d\n",bintreeGet(&r,7));
	bintreeErase(&r,0);
}
*/
