
/*
This version should add some internal commands to the shell
like cd, exit, jobs, help etc.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define PATH 256
#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30

 
int cd(char** arglist);
int execute(char* arglist[]);
char** tokenize(char* cmdline);
char* read_cmd(FILE*);
bool builtin(char **command);
char* help(char** arglist);
char* pwd();
char* username();
char* hostname();

int main(){
	char* cmdline;
	char** arglist;
	//char* prompt = PROMPT;  
	while((cmdline = read_cmd(stdin)) != NULL){
		if((arglist = tokenize(cmdline)) != NULL){
			execute(arglist);
			//  need to free arglist
			for(int j=0; j < MAXARGS+1; j++)
		         		free(arglist[j]);
		free(arglist);
		free(cmdline);
		}
	}//end of while loop
	printf("\n");
	return 0;
}
int execute(char* arglist[]){
	int status;
	if(builtin(arglist)){
		if(!strcmp(arglist[0],"cd"))	
			cd(arglist);
		else if(!strcmp(arglist[0],"exit"))
			exit(0);
			//kill(getppid(), 9); //zombie?
		else if(!strcmp(arglist[0],"jobs")){	
			chdir(arglist[1]);
		}
		else if(!strcmp(arglist[0],"help")){	
			help(arglist);
		}
	}
	else{
		int cpid = fork();
		switch(cpid){
			case -1:
				perror("Fork failed");
			      exit(1);
			case 0:
				execvp(arglist[0], arglist);
				perror("Command not found...");
				exit(1);
			default:
				waitpid(cpid, NULL, 0);
				return 0;
		}
	}
}
char** tokenize(char* cmdline){
	//allocate memory
	char** arglist = (char**)malloc(sizeof(char*)* (MAXARGS+1));
	for(int j=0; j < MAXARGS+1; j++){
		arglist[j] = (char*)malloc(sizeof(char)* ARGLEN);
		bzero(arglist[j],ARGLEN);
	}
	if(cmdline[0] == '\0')//if user has entered nothing and pressed enter key
		return NULL;
	int argnum = 0; //slots used
	char*cp = cmdline; // pos in string
	char*start;
	int len;
	while(*cp != '\0'){
		while(*cp == ' ' || *cp == '\t') //skip leading spaces
			cp++;
		start = cp; //start of the word
		len = 1;
		//find the end of the word
		while(*++cp != '\0' && !(*cp ==' ' || *cp == '\t'))
			len++;
		strncpy(arglist[argnum], start, len);
		arglist[argnum][len] = '\0';
		argnum++;
	}
	arglist[argnum] = NULL;
	return arglist;
	
}      

char* read_cmd( FILE* fp){
	printf("\n(%s@%s)-[%s]$ ", username(), hostname(), pwd());
	int c; //input character
	int pos = 0; //position of character in cmdline
	char* cmdline = (char*) malloc(sizeof(char)*MAX_LEN);
	while((c = getc(fp)) != EOF){
		if(c == '\n')
			break;
		cmdline[pos++] = c;
	}
	//these two lines are added, in case user press ctrl+d to exit the shell
	if(c == EOF && pos == 0) 
		return NULL;
	cmdline[pos] = '\0';
	return cmdline;
}


bool builtin(char **arglist){
		if(!strcmp(arglist[0],"cd") || !strcmp(arglist[0],"jobs") || !strcmp(arglist[0],"help")|| !strcmp(arglist[0],"exit"))
			return true;
		else
			return false;		
}


int cd(char** arglist){
	int cd_rv = chdir(arglist[1]);
	if (cd_rv < 0){
		perror("cd ");
		return 0;
	}	
}


char* pwd(){
	char *path = (char*)malloc(sizeof(char)* PATH); 
	path = getcwd(path,PATH); 
	if (path == NULL)
      perror("getcwd() error");
	return path;
}

char* username(){
	char *uname = getenv("LOGNAME");
	return uname;
}

char* hostname(){
	char *hostbuffer = (char*)malloc(sizeof(char)* PATH);
	gethostname(hostbuffer, sizeof(hostbuffer));
	return hostbuffer;
}

char* help(char** arglist){
	char buff[256] ;
	sprintf(buff, "cat /usr/share/zsh/help/%s", arglist[1]);	
	system(buff);
}

