#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#define MAX_LINE     80
#define MAX_COMMANDS 10

struct pastCommand
{
	char command[MAX_LINE+1];
	int index;
};


void ChildProcess(char *[]);//child process that performs execvp()
void ParentProcess(void);//parent process that reads inputs for execvp()
int extractArgs(char[], char *[]);

//PART 2
int history_counter;//counter to keep track # of commands
void historyCommand();
struct pastCommand history[MAX_COMMANDS];//list of last used commands
void rearrangeList(struct pastCommand*);

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
	char history[8] = "history";
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
			}
			else if (strcmp(history, input) == 0)
			{
				printf("User input = history\n");
				historyCommand();
			}
			else {
				printf("User did not input exit nor history\n");

				int status = extractArgs(input, args);
				if (status < 0){
					printf("An error occurred!\n");
					return -1;
				}

				struct pastCommand* new_command;
				new_command = (struct pastCommand*)malloc(sizeof(struct pastCommand));
				strcpy(new_command->command, input);
				//new_command->command = input;
				new_command->index = history_counter;
				if (new_command->command[0]=='!')
				{
					printf("history operator called, counter not increasing\n");
				}
				else
					history_counter+=1;
			
				rearrangeList(new_command);//append command to linkedlist
				
				pid = fork();
				if (pid == 0 && status != 2){
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

	printf("Checking if user called history shortcut\n");
	if ('!'==input[0])
	{
		printf("User input contains history operator: !\n");
		printf("First word of the input: %s\n", (token=strsep(&input, " ")));

		//checking whether user input is !! or !n
		char last[2] = "!!";
		if (strcmp(token, last)==0)
		{
			printf("User input is !!\n");
			printf("%i\n", history_counter);

			//will not execute anything if history is empty
			if (history_counter==0)
			{
				printf("No command has been stored\n");
				//history_counter--;
				return 2;
			}

			//find last command and replace the input with it			
			if(history_counter<MAX_COMMANDS)
			{
				input = history[history_counter-1].command;
			}
			else
			{
				input = history[MAX_COMMANDS-1].command;
			}
			printf("Calling most recent command: %s\n", input);
		}
		else//input is !n
		{
			char* substring = strtok(token, "!");
			printf("User input is !n, where n=%s\n", substring);
			
			//converting string to int
			char* base;
			int command_id;
			command_id = (int)strtol(substring, &base, 10);
			printf("Command to be executed has index=%i\n", command_id);

			//looking for corresponding command
			int range;
			if (history_counter<MAX_COMMANDS) // list not full, cannot check fron the last item
				range = history_counter;
			else
				range = MAX_COMMANDS;
			printf("Checking command range is %i\n", range);			
			
			int j=0;
			for (j;j<range;j++)
			{
				printf("Checking command with id %i\n", history[j].index);
				if (history[j].index==command_id)
				{
					input = history[j].command;
					printf("Found command with id %i, command is: %s\n", history[j].index, history[j].command);
					break;
				}
				else
				{
					if (j==range-1)//at the end of the loop
					{
						printf("Command with id %i has not been found\n", command_id);
						return 2;
					}
				}
			}
			
		}
	}

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



void historyCommand()
{
	printf("****ENTERING historyCommand()\n");
	int index;
	if (history_counter<MAX_COMMANDS)
		index=history_counter;
	else
		index=MAX_COMMANDS;

	int i;
	for(i=0;i<index;i++)
	{
		printf("%i    %s\n", history[i].index, history[i].command);
	}
}


void rearrangeList(struct pastCommand* new_command)
{
	printf("*****REARRANGING\n");
	int index;
	if (history_counter<MAX_COMMANDS)
	{
		index=history_counter;
		printf("Index chosen < max commands, list is not full: %i\n", index);
		
		if (new_command->index==0 && strcmp(new_command->command, "")==0 || new_command->command[0]=='!')
		{
			//history_counter-=1;
		}
		else
			history[index-1] = *new_command;
	}
	else
	{
		index=MAX_COMMANDS;
		printf("Index chosen = max commands, list is full, SHUFFLING: %i\n", index);
		//shuffle previously recorded commands
		int i;
		for(i=0;i<index-1;i++)
		{
			history[i] = history[i+1];
		}
		//append new commands at the end of list
		if (new_command->index==0 && strcmp(new_command->command, "")==0 || new_command->command[0]=='!')
		{
			//history_counter-=1;
		}
		else
			history[index-1] = *new_command;
	}
	

	printf("******EXITING LIST REARRANGEMENT********\n");
}
