#define DELIMITERS " \n\t|><&;"
#define BUFFER_SIZE 512
#define ARGUMENT_NUMBERS 64
#define ALIAS_FILE ".aliases"

void trimBegining(char *target);
int isAlias(char *args[ARGUMENT_NUMBERS], int argc);
int executeAlias(int aliasNum, char *args[ARGUMENT_NUMBERS], int argc);
void showAliases();
void aliasFunction(char *args[ARGUMENT_NUMBERS], int argc);
void unaliasFunction(char *args[ARGUMENT_NUMBERS], int argc);
void saveAliases();
void loadAliases();
