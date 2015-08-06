
int pgConnect(char * cparams);
int pgConnectFile(char* config);
void pgClose();
//chack connection and reconnect if needed
int pgCheck();

int pgExec(char * query);
void pgClear();

int pgRows();
int pgColumns();

//get last error
char* pgError();

//number of column by name
int pgNumber(char * name);

//size of value
int pgSize(int row, int col);

char *pgValue(int row, int col);
