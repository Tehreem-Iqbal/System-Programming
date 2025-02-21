/*
This version should add the feature of maintaining command
history and accessing them using the !n command or the up
and down arrow keys
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<fcntl.h>
#include <stdbool.h>

#define MAX_LEN 512
#define MAXARGS 50
#define ARGLEN 30
#define HISTORY_COUNT 1024
#define PATH 256

bool checkForInputRedirection(char *start ,int len);
char** parsing_commands(char** arglist);
void input_redirection(char **arglist);
int execute(char* arglist[]);
char** tokenize(char* cmdline);
char* read_cmd(FILE*);
bool builtin(char **command);
int cd(char** arglist);
char *pwd();
char *username();
char *hostname();
char* help(char** arglist);
void readcmdinhis(char **history);
	
int dp_out , dp_in;
bool bg_flag;
int k;
char** history;
void pres(int status , void *p){
	printf("\n%d %ld\n", status, (long)p);
}

int main(){
	on_exit(pres, (void*) 7);
	char *cmdline;
	char** arglist;
	//maintaing an array for history of commands
	history= (char**)malloc(sizeof(char*)* HISTORY_COUNT);
	for(int j=0; j < MAXARGS+1; j++){
		history[j] = (char*)malloc(sizeof(char)* MAXARGS);
		bzero(history[j],MAXARGS);
	}
	//Read Histroy file and store commands in array
	readcmdinhis(history);
	while((cmdline = read_cmd(stdin)) != NULL){
		if((arglist = tokenize(cmdline)) != NULL){
			parsing_commands(arglist);
			for(int j=0; j < MAXARGS+1; j++)
		         		free(arglist[j]);
		free(arglist);
		free(cmdline);
		}
	}
	printf("\n");
	return 0;
}

int execute(char* arglist[]){
	k--;
	history[k++];
	int status;
	if(builtin(arglist)){
		if(!strcmp(arglist[0],"cd"))	
			cd(arglist);
		else if(!strcmp(arglist[0],"exit")){
			hisarraytofile(history);
			exit(0);
		}
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
			perror("fork failed");
			      exit(1);
			case 0:
				execvp(arglist[0], arglist);
				perror("Command not found...");
				exit(1);
			default:
				if(bg_flag == false){
					waitpid(cpid, NULL, 0);
					return 0;
				}
				else
					return 0;
		}
	}
}
char** tokenize(char* cmdline){
	//Add the command to history array
	//allocate memory
	char** arglist1 = (char**)malloc(sizeof(char*)* (MAXARGS+1));
	char** arglist2 = (char**)malloc(sizeof(char*)* (MAXARGS+1));
	for(int j=0; j < MAXARGS+1; j++){
		arglist1[j] = (char*)malloc(sizeof(char)* ARGLEN);
		bzero(arglist1[j],ARGLEN);
		arglist2[j] = (char*)malloc(sizeof(char)* ARGLEN);
		bzero(arglist2[j],ARGLEN);
	}
	if(cmdline[0] == '\0')//if user has entered nothing and pressed enter key
		return NULL;
	int argnum1 = 0; 
	int argnum2 = 0; 
	char*cp = cmdline; 
	char*start;
	int len;
	bool rd = false; // I/O redirection flag
	while(*cp != '\0'){
		while(*cp == ' ' || *cp == '\t') 
			cp++;
		start = cp; //start of the word
		len = 1;
		//find the end of the word
		while(*++cp != '\0' && !(*cp ==' ' || *cp == '\t'))
			len++;
		if(checkForInputRedirection(start, len)) rd = true; 
		if(rd == false){
			
			strncpy(arglist1[argnum1], start, len);
			arglist1[argnum1][len] = '\0';
			argnum1++;
		}
		
		else if(rd == true){
			strncpy(arglist2[argnum2], start, len);
			arglist2[argnum2][len] = '\0';
			argnum2++;
		}
	}
	
	arglist1[argnum1] = NULL;
	arglist2[argnum2] = NULL;

	input_redirection(arglist2);
	return arglist1;
}      


char* read_cmd( FILE* fp){
	bg_flag = false;
	dup2(dp_out, 1);
	dup2(dp_in, 0);
	printf("\b(%s@%s)-[%s]$ ", username(), hostname(), pwd());
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

bool checkForInputRedirection(char *start, int len){
	char* arg = (char*)malloc(sizeof(char)* ARGLEN);
	strncpy(arg, start, len);
	if( !strcmp(arg,"1>") ||!strcmp(arg,">") || !strcmp(arg,"0<") ||!strcmp(arg,">") ||!strcmp(arg,">>") )
		return true;
	else 
		return false;

}
void input_redirection(char **arglist){
	int i = 0;
	while(arglist[i]!= NULL){
		if( !strcmp(arglist[i],"1>") ||!strcmp(arglist[i],">") ){
			dp_out = dup(1); 
			int fd_out = open(arglist[i+1], O_WRONLY | O_CREAT , 0666);
			dup2(fd_out,1);
			i++;
		}
		else if( !strcmp(arglist[i],"0<") ||!strcmp(arglist[i],">")){
			dp_in = dup(0); 
			int fd_in = open(arglist[i+1], O_RDONLY);
			dup2(fd_in,0);
			i++;
		}
		i++;
	}
}

char** parsing_commands(char** arglist){
	int iter = 0;
	int argnum;
	char **temp_buffer = (char**)malloc(sizeof(char*)* (MAXARGS+1));
	
	for(int j=0; j < MAXARGS+1; j++){
		temp_buffer[j] = (char*)malloc(sizeof(char)* ARGLEN);
		bzero(temp_buffer[j],ARGLEN);
	}
		
	while(1){
		argnum = 0;
		while((arglist[iter] != NULL) && (strcmp(arglist[iter] , ";") != 0) ){
			if (strcmp(arglist[iter], "&") != 0)
				strncpy(temp_buffer[argnum++], arglist[iter], sizeof(arglist[iter]));
			else 
				bg_flag = true;
			iter++;
		}
		temp_buffer[argnum] = NULL;
		int i = 0;
	
		execute(temp_buffer);
		if (arglist[iter] == NULL) break;
		iter++;
	}
}

bool builtin(char **arglist){
	if(!strcmp(arglist[0],"cd") || !strcmp(arglist[0],"jobs") || !strcmp(arglist[0],"help")|| !strcmp(arglist[0],"exit"))
		return true;
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

void readcmdinhis(char** history){
	int fd = open("history.txt", O_RDONLY);
	k = 0;
	while(read(fd, history[k++],MAXARGS));
}

void hisarraytofile(char** history){

	int fd = open("history.txt", O_WRONLY);
	for (int i=0;i< k; i++){
		write(fd, history[i], MAXARGS);
		write(fd, "\n", 1)
	} 
}
