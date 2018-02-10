#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define MAX_LINE     80
#define MAX_COMMANDS 5

void ChildProcess(char *[]);//child process that performs execvp()
void ParentProcess(void);//parent process that reads inputs for execvp()
int extractArgs(char[], char *[]);

int main(void)
{
	char* args[MAX_LINE/2 + 1];

	int should_run = 1;

	int i, upper;

	pid_t pid;

	char exit[5] = "exit";

	while(should_run)
	{
		printf("osh>");

		char input[MAX_LINE+1];

		if(scanf("%[^\n]%*c", input) > 0){//record user input
		    printf("Checking std.read: %s\n", input);
		    if (strcmp(exit, input) == 0)
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
			else if (pid > 0)
			  ParentProcess();
			else
			  printf("An error occurred!\n");
		      }
		}
		fflush(stdout);
		int j = 0;
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

int extractArgs(char input[], char* args[MAX_LINE/2 + 1])
{
	char * token;

	//token = strtok(input," ");
	printf("Extracting arguments\n");
	int i = 0;

	while ((token=strsep(&input, " "))!=NULL)
	{
		int j = 0;
		printf("Argument %i\n",i);
		while (token[j++] != '\0'){}
		printf("Token is %s, and has %i characters\n",token,j);
		if (( args[i] = (char*) malloc(j+1) )!=NULL){
			strcpy(args[i++],token);
		}
		else{
			printf("Unable to allocate memory!\n");
			return -1;
		}
	}
	args[i] = NULL;

	int k = 0;
	printf("i is %i, k is %i\n",i,k);
	while (k < i){
		printf("Argument %i is %s\n",k,args[k]);
		k++;
	}
	printf("Checking for & in argument %i\n",i-1);
	fflush(stdout);
	char amperstring[2] = "&";
	int background = 1;
	
	if (args[i-1] != NULL)
		background = (strcmp(args[i-1],amperstring));

	if (background == 0)
		printf("& detected\n");
	else
		printf("& NOT detected\n");
	//*counter=i;
	return background;
}









