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
	i = 0;

	while (i < (MAX_LINE/2+1))
		args[i++] = NULL;
	i = 0;
	
	pid_t pid;

	char exit[5] = "exit";
	printf("should_run is %i\n", should_run);

	while(should_run)
	{
		printf("%i osh>",getpid());

		char input[MAX_LINE+1];

		if(fgets(input, sizeof(input), stdin) != NULL){ //read user input+ check errorr
			input[strcspn(input,"\n")] = '\0';	// remove trailing newline
			
			printf("Checking std.read: %s\n", input);
			
			if (strcmp(exit, input) == 0){
				printf("User input=exit\n");
				should_run = 0;
				printf("exiting\n");
			} else {
				printf("User did not input exit\n");
				int status = extractArgs(input, args);
				if (status < 0){
					printf("An error occurred!\n");
					return -1;
				}
				
				pid = fork();
				if (pid == 0){
					ChildProcess(args);
					break;
				}	else if (pid > 0){
					if (status != 1)
						wait();
					ParentProcess();
				}	else	{
					printf("Unable to create child proces!\n");
					return -1;
				}
			}
		} else{
			printf("Input error!");
			return -1;
		}

		fflush(stdout);
	}

	return 0;
}

void ChildProcess(char *args[])
{
	printf("\nIn ChildProcess: command received is %s\n", *args);
	execvp(args[0], args);
}

void ParentProcess(void)
{
	printf("In ParentProcess\n");
}

int extractArgs(char input[MAX_LINE+1], char* args[MAX_LINE/2 + 1])
{
	char * token;

	printf("Extracting arguments\n");
	int i = 0;

	while ((token=strsep(&input, " "))!=NULL && i < (MAX_LINE/2+1))
	{
		int j = 0;
		printf("Argument %i\n",i);
		while (token[j++] != '\0'){}
		printf("Token is %s, and has %i characters\n",token,j);
		free(args[i]);
		if (( args[i] = (char*) malloc(j+1) )!=NULL){
			strcpy(args[i++],token);
		}
		else{
			printf("Unable to allocate memory!\n");
			return -1;
		}
	}
	free(args[i]);
	args[i] = NULL;

	int j = 0;
	printf("i is %i, j is %i\n",i,j);
	while (j < i){
		printf("Argument %i is %s\n",j,args[j]);
		j++;
	}
	printf("Checking for & in argument %i\n",i-1);
	
	char amperstring[2] = "&";
	int background = (strcmp(args[i-1],amperstring));

	if (background == 0){
		printf("& detected");
		free(args[i-1]);
		args[i-1] = NULL;
		return 1;
	} else{
		printf("& NOT detected");
		return 0;
	}
}

