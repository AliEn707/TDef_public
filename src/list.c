#include "main.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ 		functions for l1 list with data 				                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/




worklist * worklistAdd(worklist * root, int id){
	worklist * tmp;
	for(tmp=root;tmp->next!=0;tmp=tmp->next);
	if ((tmp->next=malloc(sizeof(worklist)))==0){
		perror("malloc worklistAdd");
		return 0;
	}
	memset(tmp->next,0,sizeof(worklist));
	tmp->next->id=id;
	return tmp->next;
}

worklist * worklistDel(worklist * root, int id){
	worklist* tmp, * tmp1;
	for(tmp=root;tmp->next!=0;tmp=tmp->next)
		if (tmp->next->id==id)
			break;
	if (tmp->next==0)
		return 0;
	tmp1=tmp->next;
	tmp->next=tmp->next->next;
	free(tmp1);
	return tmp;
}

void worklistErase(worklist* root){
	if (root==0)
		return;
	worklistErase(root->next);
	free(root->next);
	root->next=0;
}

/*
int main(){
	worklist l;
	memset(&l,0,sizeof(l));
	worklistAdd(&l,5);
	worklistAdd(&l,7);
	worklistDel(&l,7);
	worklistErase(&l);
}
*/