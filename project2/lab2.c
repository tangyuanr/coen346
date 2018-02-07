#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define MAX_LINE     80
#define MAX_COMMANDS 5

//bool compareString(char *[], char *[]);
void ChildProcess(char *[]);//child process that performs execvp()
void ParentProcess(void);//parent process that reads inputs for execvp()
int extractArgs(char[], char *[]);

int main(void)
{
	char *args[MAX_LINE/2 + 1];

	int should_run = 1;

	int i, upper;

	pid_t pid;
	
	

	char exit[5];
	strcpy(exit, "exit");

	while(should_run)
	{
		printf("osh>");

		char input[MAX_LINE];

		scanf("%[^\n]%*c", input);//record user input
		//read(STDIN_FILENO, input, MAX_LINE);
		printf("Checking std.read: %s\n", input);
		if (0==strcmp(exit, input))
		{
			printf("User input=exit\n");
			should_run=0;
			printf("exiting\n");
			break;
		}
		else
		{
			printf("User did not input exit\n");
			int status = extractArgs(input, args);
			
			pid = fork();
			if (pid==0)
				ChildProcess(args);
			else
				ParentProcess();
		}

		fflush(stdout); 
	}

	return 0;
}

void ChildProcess(char *args[])
{
	printf("In ChildProcess: input received is %s\n", *args);
	execvp(args[0], args);

}

void ParentProcess(void)
{
	printf("In ParentProcess\n");
}

int extractArgs(char input[], char *args[])
{
	char * token;
	//token = strsep(input," ");
	printf("Extracting arguments\n");
	int i = 0;

	while ((token=strsep(&input, " "))!=NULL)
	{
		malloc(sizeof(MAX_LINE/2 + 1));
		printf("Argument %i\n",i);
		args[i++]=token;
	}
/*
	while (token != NULL)
	{
		malloc(sizeof(MAX_LINE/2 + 1));
		printf("Argument %i\n",i);
		strcpy(*(args+i*(MAX_LINE/2 + 1)),token);
		i++;
		token = strtok(NULL," ");
	}
*/
	int j =0;
	while (j < i)
	{
		printf("%s\n",*(args+j*(MAX_LINE/2 + 1)));
		j++;
	}
	int background = ('&'== *(*(args+(i-1)*(MAX_LINE/2 + 1))));
	if (background)
	{
		printf("& detected\n");
	}

	//*counter=i;
	return background;
}









