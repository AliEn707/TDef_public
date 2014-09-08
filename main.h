#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef 
struct bintree{
	void * data;
	struct bintree * next[2];
} bintree;

typedef
struct worklist{
	int id;
	struct worklist * next;
} worklist;