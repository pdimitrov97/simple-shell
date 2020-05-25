#define DELIMITERS " \n\t|><&;"
#define BUFFER_SIZE 512
#define ARGUMENT_NUMBERS 64
#define HISTORY_FILE ".hist_list"

void trimBegining(char *target);
void shellLoop(void);
int executeCommand(char *args[], int argc);
int exitShell(int argc);
void changeDirectory(char *args[], int argc);
void getPath(int argc);
void setPath(char *args[], int argc);
void showHistory(int argc);
int historyFunction(char *args[], int argc);
void executeExternal(char *args[]);
void saveHistory();
void loadHistory();

