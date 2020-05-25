#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "aliases.h"

struct Alias {
	char key[BUFFER_SIZE];
	char data[BUFFER_SIZE];
};

struct Alias aliases[10];

int aliasLoop[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int aliasStarted = 0;

/*
	This function checks if a given input maps to an alias.

	parameters:
	args - array with the tokenized arguments from the input
	argc - the number of arguments passed in the input
*/
int isAlias(char *args[ARGUMENT_NUMBERS], int argc)
{
	for (int i = 0 ; i < 10 ; i++)
	{
		if (strcmp(args[0], aliases[i].key) == 0)
			return i;
	}

	return -1;
}

/*
	This function executes an alias with a given number and adds the arguments entered by the user.

	parameters:
	aliasNum - the number of the alias to be executed
	args - array with the tokenized arguments from the input
	argc - the number of arguments passed in the input
*/
int executeAlias(int aliasNum, char *args[ARGUMENT_NUMBERS], int argc)
{
	char tempAlias[BUFFER_SIZE];
	char *tempArgs[ARGUMENT_NUMBERS];
	int tempArgc = 0;
	int localStarted = 0;
	int status;

	if (aliasNum < 0 || aliasNum > 10)
	{
		printf("Something went wrong with aliases !n");
		return 0;
	}

	if (strcmp(args[0], aliases[aliasNum].key) == 0)
	{
		strcpy(tempAlias, aliases[aliasNum].data);
		tempArgs[tempArgc] = strtok(tempAlias, DELIMITERS);
		
		while (tempArgs[tempArgc] != NULL)
			tempArgs[++tempArgc] = strtok(NULL, DELIMITERS);
		
    		for (int k = 1 ; k < argc ; k++)
		{
			tempArgs[tempArgc] = malloc(sizeof(char*));
			strcpy(tempArgs[tempArgc], args[k]);
			tempArgc++;
		}

		tempArgs[tempArgc] = NULL;
		
		if (aliasStarted == 0)
		{
			localStarted = 1;
			aliasStarted = 1;
		}
		
		aliasLoop[aliasNum]++;
		
		for (int j = 0 ; j < 10 ; j++)
		{
			if (aliasLoop[j] > 1)
			{
				printf("alias: Loop detected!\n");
				return 0;
			}
		}

		status = executeCommand(tempArgs, tempArgc);

		if (localStarted)
		{
			for (int j = 0 ; j < 10 ; j++)
				aliasLoop[j] = 0;

			aliasStarted = 0;
		}

		return status;
	}
	else
	{
		printf("Something went wrong with aliases !n");
		return 0;
	}
}

/*
	This function prints all aliases the user has done.
*/
void showAliases()
{
	int flag = 0;
	
	for (int i = 0 ; i < 10 ; i++)
	{
		if (strcmp(aliases[i].key, "") != 0)
		{
			printf("%s - %s\n", aliases[i].key, aliases[i].data);
			flag++;
		}
	}
	
	if (flag == 0)
		printf("No aliases set!\n");
}

/*
	This function handles creating new aliases.

	parameters:
	buffer - the whole line the user has entered.
*/
void aliasFunction(char *args[ARGUMENT_NUMBERS], int argc)
{
	if (argc == 1)
	{
		showAliases();
		return;
	}
		
	if (argc < 3)
	{
		printf("alias: <name> <command>\n");
		return;
	}
	
	char tempData[BUFFER_SIZE] = "";
	
	for (int i = 2 ; i < argc ; i++)
	{
		strcat(tempData, args[i]);
		
		if(i < (argc - 1))
			strcat(tempData, " ");
	}
	
	for (int i = 0 ; i < 10 ; i++)
	{
		if (strcmp(args[1], aliases[i].key) == 0)
		{
			strcpy(aliases[i].data, tempData);
			printf("Alias \"%s\" overwritten to \"%s\" !\n", aliases[i].key, aliases[i].data);
			return;
		}
	}
	
	for (int i = 0 ; i < 10 ; i++)
	{
		if (strcmp(aliases[i].key, "") == 0)
		{
			strcpy(aliases[i].key, args[1]);
			strcpy(aliases[i].data, tempData);
			printf("Alias \"%s\" set to \"%s\" !\n", aliases[i].key, aliases[i].data);
			return;
		}
	}
	
	printf("Max aliases reached !\n");
}

/*
	This function handles deleting aliases.

	parameters:
	buffer - the whole line the user has entered.
*/
void unaliasFunction(char *args[ARGUMENT_NUMBERS], int argc)
{
	if (argc != 2)
	{
		printf("unalias: <name>\n");
		return;
	}
	
	for (int i = 0 ; i < 10 ; i++)
	{
		if (strcmp(args[1], aliases[i].key) == 0)
		{
			strcpy(aliases[i].key, "");
			strcpy(aliases[i].data, "");
			printf("Alias \"%s\" removed !\n", args[1]);
			return;
		}
	}
	
	printf("Alias \"%s\" not found !\n", args[1]);
}

/*
	This function is used to save user's aliases into a file.
*/
void saveAliases()
{
	FILE *file_ptr;
	file_ptr = fopen(ALIAS_FILE, "w+");

	if(file_ptr == NULL)
	{
		printf("Error creating file - %s\n", ALIAS_FILE);
		return;
	}

	for (int i = 0 ; i < 10 ; i++)
	{
		if (strcmp(aliases[i].key, "") != 0)
		{
			fputs(aliases[i].key, file_ptr);
			fputs(" ", file_ptr);
			fputs(aliases[i].data, file_ptr);
			fputs("\n", file_ptr);
		}
	}

	fclose(file_ptr);
}

/*
	This function is used to load user's aliases from a file.
*/
void loadAliases()
{
	FILE *file_ptr;
	file_ptr = fopen(ALIAS_FILE ,"r");
	int count = 0;
	char *tempKey;
	char *tempData;

	if(file_ptr == NULL)
	{
		printf("Cannot find file at - %s\n", ALIAS_FILE);
		return;
	}

	char buffer[BUFFER_SIZE];

	while(fgets(buffer, BUFFER_SIZE, file_ptr) != NULL)
	{
		if(count < 10)
		{
			buffer[strlen(buffer) - 1] = '\0'; // remove newline char
			trimBegining(buffer);

			if (strlen(buffer) == 0)
				continue;
			
			tempKey = strtok(buffer, " ");

			if (tempKey == NULL)
				continue;

			tempData = strtok(NULL, "");

			if (tempData == NULL)
				continue;
			
			strcpy(aliases[count].key, tempKey);
			strcpy(aliases[count].data, tempData);
			
			count++;
		}
		else
			printf("Error reading aliases file!\nToo many lines!\n");
	}
	
	printf("Reading from file \"%s\" completed !\n%d alias%s loaded\n", ALIAS_FILE, count, count == 1 ? "" : "es");	
	fclose(file_ptr);
}
