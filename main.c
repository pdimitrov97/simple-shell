#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "shell.h"
#include "aliases.h"

void trimBegining(char *target);

/*
	This program is a simple shell. This function stores the current path and change it to HOME.
	Then it runs the actual shell and after the shell function finishes, it restores the original path.
*/
int main()
{
	char originalPath[PATH_MAX];
	
	printf("\n\nStarting Shell...\n");
	strcpy(originalPath, getenv("PATH")); // Save the current path
	printf("\nOriginal system path is: %s\n", originalPath);
		
	shellLoop();
	
	setenv("PATH", originalPath, 1);
	printf("\nOriginal system path restored: %s\n\n", getenv("PATH"));

	if (chdir(getenv("HOME")) != 0)
		printf("Cannot get HOME directory !\n");  
	
	saveHistory();
	saveAliases();
        
	return 0;
}

/*
	This function removes every space character from the begining of a given string

	parameters:
	target - the string to be modified
*/
void trimBegining(char *target)
{
	char *temp = target;
	int i = 0;

	while(temp[i] == ' ')
		i++;

	memcpy(target, temp + i, strlen(temp) - i);
	target[strlen(temp) - i] = '\0';
}

