int dbConnect(char * cparams);

void dbClose();

int dbCheck();

int dbExec(char * query);

int dbRows();

int dbColumns();

char* dbError();

//number of column by name
int dbNumber(char * name);

//size of value
int dbSize(int row, int col);

char *dbValue(int row, int col);
