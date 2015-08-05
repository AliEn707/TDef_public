
int dbConnect(char * cparams);
void dbClose();
//chack connection and reconnect if needed
int dbCheck();

int dbExec(char * query);
void pgClear();

int dbRows();
int dbColumns();

//get last error
char* dbError();

//number of column by name
int dbNumber(char * name);

//size of value
int dbSize(int row, int col);

char *dbValue(int row, int col);
