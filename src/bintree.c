#include "main.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ 		functions for binary tree with data 			                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


int bintreeAdd(bintree* root,int key,void* data){
	bintree* tmp=root;
	int next;
	printf("add key %d\n",key);
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

int _bintreeDel(bintree* root, int key){
	int get;
	int next;
	if (root==0)
		return 0;
	if (key==0){
//		get=root->data;
//		free(root->data); //may be need
		root->data=0;
		if (root->next[0]==0 && root->next[1]==0){
			free(root);
			printf("free\n");
			return 1;
		}
		return 0;
	}
	next=key&1;
	get=_bintreeDel(root->next[next],key>>1);
//	free(root->next[next]);
	if (get!=0){
		root->next[next]=0;
		if (root->next[(next+1)%2]==0){
//			printf("%d %d\n",next,(next+1)%2);
			free(root);
			return 1;
		}
	}
	return 0;
}

int bintreeDel(bintree* root, int key){
	int get;
	int next=key&1;
	get=_bintreeDel(root->next[next],key>>1);
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
void bintreeErase(bintree * root){
	if (root==0)
		return;
//	printf("y\n");
	bintreeErase(root->next[0]);
	bintreeErase(root->next[1]);
	free(root->next[0]);
	root->next[0]=0;
	free(root->next[1]);
	root->next[1]=0;
}

/*
int main(){
	bintree r;
	memset(&r,0,sizeof(r));
	bintreeAdd(&r,3,100);
	bintreeAdd(&r,7,132);
	printf("%d\n",bintreeGet(&r,3));
	bintreeDel(&r,3);
//	bintreeDel(&r,3);
	printf("%d\n",bintreeGet(&r,3));
	printf("%d\n",bintreeGet(&r,7));
	bintreeErase(&r);
}
*/