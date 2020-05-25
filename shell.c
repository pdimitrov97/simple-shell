#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <values.h>
#include <linux/limits.h>

#include "shell.h"
#include "aliases.h"

char historyCommands[20][BUFFER_SIZE];
int historyCounter = 1;

/*
	This function is the main function of the shell that handles input and it is running all of the time.
*/
void shellLoop(void)
{
	int argc = 0;
	char *args[ARGUMENT_NUMBERS];
	char buffer[BUFFER_SIZE + 1];
	char currentPath[PATH_MAX];

	printf("\nShell started !\n\n");
	chdir(getenv("HOME")); // Set current working directory to user home directory

	loadHistory();
	loadAliases();

	while (1)
	{
		argc = 0;
		getcwd(currentPath, PATH_MAX);
		printf("%s>", currentPath);

		if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
			break;
            
		buffer[strlen(buffer) - 1] = '\0'; // remove newline char
		trimBegining(buffer);
	
		if (strncmp(buffer, "!", 1) != 0 && buffer[0] != '\0')
		{
			strcpy(historyCommands[(historyCounter - 1) % 20], buffer);
			historyCounter++;
		}

		args[argc] = strtok(buffer, DELIMITERS);

		while (args[argc] != NULL)
			args[++argc] = strtok(NULL, DELIMITERS);

		if(executeCommand(args, argc) == 1)
			break;
	}

	printf("\n\nShutting down Shell...\n");
}

/*
	This function executes a given command that was passed.

	parameters:
	args - array with the tokenized arguments from the input
	argc - the number of arguments passed in the input
*/
int executeCommand(char *args[ARGUMENT_NUMBERS], int argc)
{
	int aliasNum;	

	if (argc > 0)
	{
		aliasNum = isAlias(args, argc);
	
		if (aliasNum > -1)
			return executeAlias(aliasNum, args, argc);
		else if (strcmp(args[0], "exit") == 0)
			return exitShell(argc);
		else if (strcmp(args[0], "cd") == 0)
			changeDirectory(args, argc);
		else if (strcmp(args[0], "getpath") == 0)
			getPath(argc);
		else if (strcmp(args[0], "setpath") == 0)
			setPath(args, argc);
		else if (strcmp(args[0], "history") == 0)
			showHistory(argc);
		else if (strncmp(args[0], "!", 1) == 0)
			return historyFunction(args, argc);
		else if (strcmp(args[0], "alias") == 0)
			aliasFunction(args, argc);
		else if (strcmp(args[0], "unalias") == 0)
			unaliasFunction(args, argc);
		else
			executeExternal(args);
	}

	return 0;
}

/*
	This function makes the shell to terminate.

	parameters:
	argc - the number of arguments passed in the input
*/
int exitShell(int argc)
{
	if (argc > 1)
	{
		printf("exit: No arguments accepted!\nExit from Shell.\n");
		return 0;
	}
	else
		return 1;
}

/*
	This function changes the the current working directory of the shell.

	parameters:
	args - array with the tokenized arguments from the input
	argc - the number of arguments passed in the input
*/
void changeDirectory(char *args[ARGUMENT_NUMBERS], int argc)
{
	if (argc > 2)
		printf("cd: Usage: cd [PATH]\nChange working directory.\n");
	else
	{
		if (args[1] == NULL)
		{
			if (chdir(getenv("HOME")) != 0)
				perror(args[1]);  
		}
		else
		{
			if (chdir(args[1]) != 0)
				perror(args[1]);
		}
	}
}

/*
	This function gets the current system path and prints it.

	parameters:
	argc - the number of arguments passed in the input
*/
void getPath(int argc)
{
	if (argc > 1)
		printf("getpath: No arguments accepted!\n");
	else
		printf("Current system path: %s\n", getenv("PATH"));
}

/*
	This function sets the current system path.

	parameters:
	args - array with the tokenized arguments from the input
	argc - the number of arguments passed in the input
*/
void setPath(char *args[ARGUMENT_NUMBERS], int argc)
{
	if(argc > 2 || args[1] == NULL)
		printf("setpath: Usage: setpath [PATH]\n");
	else
	{
		if (setenv("PATH", args[1] , 1) == 0)
			printf("System path successfully set to: %s\n", args[1]);
		else
			printf("Error setting system path !\n");
	}
}

/*
	This function prints the content of the history to the user.

	parameters:
	argc - the number of arguments passed in the input
*/
void showHistory(int argc)
{
	if (argc > 1)
		printf("history: No arguments accepted!\nShows Shell\'s command history.\n");
	else
	{
		printf("\nShell History:\n");

		if (historyCounter <= 20)
		{
			for (int i = 0 ; i < 20 && i < historyCounter - 1 ; i++)
			{
				printf("%2d - %s\n", (i + 1), historyCommands[i]);
			}
		}
		else
		{
			for (int i = 0 ; i < 20 ; i++)
			{
				printf("%2d - %s\n", (i + 1), historyCommands[(((historyCounter - 1) % 20) + i) % 20]);
			}
		}
	}
}

/*
	This function handles history invocations of the type !!, !number, !-number.

	parameters:
	args - array with the tokenized arguments from the input
	argc - the number of arguments passed in the input
*/
int historyFunction(char *args[], int argc)
{
	char tempHistoryCommand[BUFFER_SIZE];
	char *tempArgs[ARGUMENT_NUMBERS];
	int tempArgc = 0;

	if (argc > 1)
	{
		printf("History invocations do not require parameters !\n");
		return -1;		
	}

	if (strcmp(args[0], "!!") == 0)
	{
		if (historyCounter <= 1)
		{
			printf("No last command in Shell\'s history !\n");
			return -1;		
		}

		strcpy(tempHistoryCommand, historyCommands[(historyCounter - 2) % 20]);
		tempArgs[tempArgc] = strtok(tempHistoryCommand, DELIMITERS);
	}
	else if (args[0][1] == '-')
	{
		char *endPointer;
		char tempTextNumber[4];
		int tempNumber = 0;

		if (strlen(args[0]) > 4)
		{
			printf("History invocation require only two-digit number !\n");
			return 0; 
		}  
    	
		strncpy(tempTextNumber, args[0] + 2, 2);

		if (tempTextNumber[0] < '0' || tempTextNumber[0] > '9' || (tempTextNumber[1] < '0' || tempTextNumber[1] > '9') && tempTextNumber[1] !='\0')
		{
			printf("History invocations require only numbers !\n");
			return 0;
		}

		tempNumber = strtol(tempTextNumber, &endPointer, 10);

		if (tempNumber <= 0 || tempNumber > 20)
		{
			printf("Shell\'s history is only 20 commands and starts from 1 !\n");
			return -1;
		}

		if (historyCounter <= tempNumber)
		{
			printf("Command is NOT in history !\n");
			return -1;
		}
		
		strcpy(tempHistoryCommand, historyCommands[(historyCounter - tempNumber - 1) % 20]);
		tempArgs[tempArgc] = strtok(tempHistoryCommand, DELIMITERS);
	}
	else
	{
		char *endPointer;
		char tempTextNumber[4];
		int tempNumber = 0;

		if (strlen(args[0]) > 3)
		{
			printf("History invocation require only two-digit number !\n");
			return 0; 
		}   

		strncpy(tempTextNumber, args[0] + 1, 2);

		if (tempTextNumber[0] < '0' || tempTextNumber[0] > '9' || (tempTextNumber[1] < '0' || tempTextNumber[1] > '9') && tempTextNumber[1] !='\0')
		{
			printf("History invocations require only numbers !\n");
			return 0;
		}

		tempNumber = strtol(tempTextNumber, &endPointer, 10);

		if (tempNumber <= 0 || tempNumber > 20)
		{
			printf("Shell\'s history is only 20 commands and starts from 1 !\n");
			return -1;
		}

		if (historyCounter <= tempNumber)
		{
			printf("Command is NOT in history !\n");
			return -1;
		}

		if (historyCounter <= 20)
			strcpy(tempHistoryCommand, historyCommands[tempNumber - 1]);
		else    	
			strcpy(tempHistoryCommand, historyCommands[((((historyCounter - 1) % 20) + tempNumber - 1) % 20)]);

		tempArgs[tempArgc] = strtok(tempHistoryCommand, DELIMITERS);
	}

	while (tempArgs[tempArgc] != NULL)
		tempArgs[++tempArgc] = strtok(NULL, DELIMITERS);

	return executeCommand(tempArgs, tempArgc);
}

/*
	This function is used to executes an external program.

	parameters:
	args - array with the tokenized arguments from the input
*/
void executeExternal(char *args[ARGUMENT_NUMBERS])
{
	pid_t pid = fork();
            
	if (pid == 0)
	{
		execvp(args[0], args);
		perror(args[0]);
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
		printf("ERROR \n");
	else
		wait(NULL);
}

/*
	This function is used to save user's history into a file.
*/
void saveHistory()
{
	FILE *file_ptr;
	file_ptr = fopen(HISTORY_FILE, "w+");

	if(file_ptr == NULL)
	{
		printf("Error creating file - %s\n", HISTORY_FILE);
		return;
	}
	
	if (historyCounter <= 20)
	{
		for (int i = 0 ; i < 20 && i < historyCounter - 1 ; i++)
		{
			fputs(historyCommands[i], file_ptr);
			fputs("\n", file_ptr);
		}
	}
	else
	{
		for (int i = 0 ; i < 20 ; i++)
		{
			fputs(historyCommands[(((historyCounter - 1) % 20) + i) % 20], file_ptr);
			fputs("\n", file_ptr);
		}
	}
	
	fclose(file_ptr);
}

/*
	This function is used to load user's history from a file.
*/
void loadHistory()
{
	FILE *file_ptr;
	file_ptr = fopen(HISTORY_FILE ,"r");

	if(file_ptr == NULL)
	{
		printf("Cannot find file at - %s\n", HISTORY_FILE);
		return;
	}

	char buffer[BUFFER_SIZE];

	while(fgets(buffer, BUFFER_SIZE, file_ptr) != NULL)
	{
		if(historyCounter <= 20)
		{
			buffer[strlen(buffer) - 1] = '\0'; // remove newline char
			trimBegining(buffer);

			if (strlen(buffer) == 0)
				continue;

			strcpy(historyCommands[historyCounter - 1], buffer);
			historyCounter++;
		}
		else
			printf("Error reading history file!\nToo many lines!\n");
	}
	
	printf("Reading from file \"%s\" completed !\n%d command%s loaded\n", HISTORY_FILE, (historyCounter - 1), (historyCounter - 1) == 1 ? "" : "s");	
	fclose(file_ptr);
}
