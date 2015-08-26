worklist * worklistAdd(worklist * root, int id);

worklist * worklistDel(worklist * root, int id);

void worklistErase(worklist* root);

// remove object if f return non zero
void worklistForEach(worklist* root, void*(f)(worklist* w));