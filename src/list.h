typedef
struct worklist{
	int sock; //client socket
	int id;  //client id
	void * data;
	struct worklist * next;
} worklist;

worklist * worklistAdd(worklist * root, int id);

worklist * worklistDel(worklist * root, int id);

void worklistErase(worklist* root);

// remove object if f()!=0
void worklistForEachRemove(worklist* root, void*(f)(worklist* w, void* arg), void* arg);

//return if f()!=0
void* worklistForEachReturn(worklist* root, void*(f)(worklist* w, void* arg), void* arg);
