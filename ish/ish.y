%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include "y.tab.h"
#include "ish.h"

%}

%union
{
    char	*string;
    int		integer;
}

%token 	<string>	WORD
%token 	<string>	COMMAND
%token 	<string>	FILENAME
%token	<int>		BACKGROUND
%token	<int>		PIPE
%token	<int>		PIPE_ERROR
%token	<int>		SEMICOLON
%token	<int>		REDIRECT_IN
%token	<int>		REDIRECT_OUT
%token	<int>		REDIRECT_ERROR
%token	<int>		APPEND
%token	<int>		APPEND_ERROR
%token	<string>	OPTION
%token	<string>	STRING
%token	<int>		LOGICAL_AND
%token	<int>		LOGICAL_OR
%token	<int>		NEWLINE

%%

cmd_line 	: cmd_line separator COMMAND parameters { printf("%s\n", $3); strcpy(commandsArray[commandCount++], $3);  }
        | COMMAND parameters  { printf("%s\n", $1);strcpy(commandsArray[commandCount++], $1);  }
		| cmd_line BACKGROUND
		| cmd_line SEMICOLON
		|  
		| error 
		;

separator 	: BACKGROUND 
		| PIPE
		| PIPE_ERROR
		| SEMICOLON {  printf("semicolon\n");  counts[commandCount-1] = paramCount; printf("parameter count = %d\n",counts[commandCount-1] ); paramCount = 0;  parameterCount = commandCount;}
		;

parameters	: parameters OPTION { printf("OPTION %s\n", $2); strcpy(parametersArray[parameterCount][paramCount++], $2); }
		| parameters STRING { printf(" STRING %s\n", $2); strcpy(parametersArray[parameterCount][paramCount++], $2); }
		| parameters WORD { printf("WORD %s\n", $2); strcpy(parametersArray[parameterCount][paramCount++], $2); }
		| parameters REDIRECT_IN FILENAME
        | parameters REDIRECT_OUT FILENAME
		| parameters REDIRECT_ERROR FILENAME
		| parameters APPEND FILENAME
		| parameters APPEND_ERROR FILENAME
		|
		;

%%

int yyerror(char *s)
{
    fprintf(stderr, "syntax error\n");
    return 0;
}


int main(){

	while(1){
		
		ish_init();
	
        switch(getCommand()) {
			//printf("%d\n",getCommand());
			case YYerror:
				printf("ERROR\n");
				break;
			case F_OK:
                printf("OK\n");	
				process();
				destroy();
				break;
			default: 
				printf("uhhh...whoops.\n");				
				break;
		}
	}
	printf("\n");
	return 0;
}




char* hostname(){
	char *hostbuffer = (char*)malloc(sizeof(char)* PATH);
	gethostname(hostbuffer, PATH);
	return hostbuffer;
}

int getCommand() {
	if(!yyparse()) //return zero on success
		return F_OK;
	else 
		return YYerror;
}


void ish_init(){
		
		printf("\n");
	    printf("[1;34;40m" "%s : ", hostname());

		commandsArray = (char**) malloc(COMMANDS * sizeof(char*));
		parametersArray = (char***) malloc(COMMANDS * sizeof(char**));
		for(int j=0; j < COMMANDS; j++){
			commandsArray[j] = (char*)malloc(sizeof(char)* ARGLEN);
			bzero(commandsArray[j],ARGLEN);	
		}
		for(int i=0; i < COMMANDS; i++){
			parametersArray[i] = (char**)malloc(sizeof(char*)* PARAMETERS);
			for (int p=0; p< PARAMETERS;p++){
				parametersArray[i][p] = (char*)malloc(sizeof(char)* ARGLEN);
				memset(parametersArray[i][p], 0,ARGLEN );
			//bzero(parametersArray[i][p],ARGLEN);
			//parametersArray[i][p] = NULL;
			}
		}
			wordCount = 0; 
			commandCount = 0;
			parameterCount = 0;
			paramCount = 0;
}

void process(){
	int i;
	for(i=0;i<commandCount-1;i++){
		printf("-----------------");
		printf("command %d : %s\n", i, commandsArray[i]);
		for(int p=0; p< counts[commandCount] ;p++)
			printf("command : %d , para %d : %s\n ", i, p, parametersArray[i][p] );
		printf("-----------------");
	}
	for(int p=0; p< paramCount ;p++)
			printf("command : %d , para %d : %s\n ", i, p, parametersArray[i][p] );

	for(i=0;i<commandCount;i++){
	
		if(isBuiltin(commandsArray[i])) {
			
			if(!strcmp(commandsArray[i],"cd"))	
				cd(parametersArray[i][0]);
			else if(!strcmp(commandsArray[i],"exit"))	
				bye();
			else if(!strcmp(commandsArray[i],"setenv"))	
				setEnv(parametersArray[i][0], parametersArray[i][1]);
			else if(!strcmp(commandsArray[i],"unsetenv"))	
				unsetEnv(parametersArray[i][0]);
			else if(!strcmp(commandsArray[i],"jobs"))	
				printf("Jobs");
			else if(!strcmp(commandsArray[i],"bg"))	
				printf("bg");
			else if(!strcmp(commandsArray[i],"fg"))	
				printf("fg");
		}
			
		else{
			
			executeCommand(i, commandsArray[i], parametersArray[i]);
		}
	}
}


bool isBuiltin(char* command){

	if(!strcmp(command,"cd") || !strcmp(command,"jobs") || !strcmp(command,"bg")|| !strcmp(command,"fg") || !strcmp(command,"kill") || !strcmp(command,"setenv") || !strcmp(command,"unsetenv"))
		return true;
	return false;
}
int cd(char* arg){
	int cd_rv = chdir(arg);
	if (cd_rv < 0){
		perror("cd ");
		return 0;
	}
}

void bye(){	
	printf("[01;31;40m" "\n******************************  SHELL ENDS HERE  *******************************\n\n");
	printf("[00;00;00m");
	exit(0); //exit shell
		
}

void printEnv(){
    char **env = environ;
    // Loop through each environment variable and print its name and value
    while (*env != NULL) {
        printf("%s\n", *env);
        env++;
    }
}

void setEnv(char* var, char* value)
{
	//printEnv();
	setenv(var, value, 0);
	char *val = getenv(var);
	printf("Environment variable set\n %s = %s\n", var, val);
}
void unsetEnv(char* var)
{
	unsetenv(var);
	printf("Successfully unset variable");
}


int executeCommand(int c, char* command, char** args){

	arglist = (char**)malloc(COMMANDS*sizeof(char));
	for(int j=0; j < COMMANDS; j++){
		arglist[j] = (char*)malloc(sizeof(char)* COMMANDS);
		bzero(arglist[j],COMMANDS);
	}
	
	strcpy(arglist[0], command);

	printf("no of args: %d\n", counts[c]);
	int i;
	for(i=0 ;i<counts[c];i++){
		strcpy(arglist[i+1], args[i]);
		printf("%s\n",arglist[i+1]);
	}
	//arglist[i+1] = NULL;
	printf("COMMAND %s\n", arglist[0]);
	
	int cpid = fork();
		switch(cpid){
			case -1:
			perror("fork failed");
					exit(1);
			case 0:
				for(int i=0 ;i<10;i++)
					printf("%s\n",arglist[i]);
				execvp(arglist[0], arglist);
				perror("Command not found.");
				exit(1);
			default:
				waitpid(cpid, NULL, 0);
				return 0;
		}
}

void destroy(){
	for (int j = 0; j < COMMANDS; j++) 
		free(commandsArray[j]);

	
	
	free(commandsArray);


	
	for (int i = 0; i < COMMANDS; i++) {
		for (int p = 0; p < PARAMETERS; p++) {
			
			free(parametersArray[i][p]);
		}
		free(parametersArray[i]);
	}
	
	free(parametersArray);
	
}
