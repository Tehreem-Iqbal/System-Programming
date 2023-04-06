
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LEN 1024
#define MAXARGS 50
#define ARGLEN 30
#define COMMANDS_COUNT 1024
#define PATH 256
char* hostname();
int getCommand();
/*
bool checkForInputRedirection(char *start ,int len);
char** parsing_commands(char** arglist);
void input_redirection(char **arglist);
int execute(char* arglist[]);
char** tokenize(char* cmdline);
char* read_cmd();
bool builtin(char **command);
int cd(char** arglist);
char *pwd();
char *username();

void help(char* arglist);
void readcmdsfromhis(char **history);
void hisarraytofile(char **history);
void executeHistory(char* cmd);
void showHistory();
int up_arrow_fun(int count, int key);
int down_arrow_fun(int count, int key);
char* checkVar(char *buffer);

int dp_out , dp_in;
bool bg_flag;
int k;
*/

int main(){
	
	char* cmdline;
	char** arglist;

	while(1){
	    printf("[1;34;40m" "%s:", hostname());
        switch(getCommand()) {
			case YYerror:
				printf("You did something wrong.\n");
				break;
			case OK:
                printf("uhhh...whoops.\n");	
				//processCommand();
				break;
			default: 
				printf("uhhh...whoops.\n");				
				break;
		}
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
		return OK;
	else 
		return YYerror;
}
/*
int execute(char* arglist[]){
	
	if(arglist[0][0] == '!'){
		int n = ((int)arglist[0][1]) - 48;
		HIST_ENTRY *entry = history_get(n);
		printf("%s\n", entry->line);
		executeHistory(entry->line);
	}
	else if (arglist[1] != NULL && !strcmp(arglist[1], "=")){
			setenv(arglist[0], arglist[2],1);
	}
	else{
		if(builtin(arglist)){
			if(!strcmp(arglist[0],"cd"))	
				cd(arglist);
			else if(!strcmp(arglist[0],"exit")){
				write_history("history"); 
				exit(0);
			}
			else if(!strcmp(arglist[0],"jobs"))	
				printf("Jobs");
			
			else if(!strcmp(arglist[0],"help"))	
				help(arglist[1]);
			
			else if(!strcmp(arglist[0],"history"))
				showHistory();
			
		}
		else{
			int cpid = fork();
			switch(cpid){
				case -1:
				perror("fork failed");
				      exit(1);
				case 0:
					execvp(arglist[0], arglist);
					perror("Command not found.");
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
}

char** tokenize(char* cmdline){
	
	char** arglist1 = (char**)malloc(sizeof(char*)* (MAXARGS+1));
	char** arglist2 = (char**)malloc(sizeof(char*)* (MAXARGS+1));
	for(int j=0; j < MAXARGS+1; j++){
		arglist1[j] = (char*)malloc(sizeof(char)* ARGLEN);
		bzero(arglist1[j],ARGLEN);
		arglist2[j] = (char*)malloc(sizeof(char)* ARGLEN);
		bzero(arglist2[j],ARGLEN);
	}
	if(cmdline[0] == '\0')
		return NULL;
	int argnum1 = 0; 
	int argnum2 = 0; 
	char*cp = cmdline; 
	char*start;
	int len;
	bool rd = false; 
	while(*cp != '\0'){
		while(*cp == ' ' || *cp == '\t') 
			cp++;
		start = cp; 
		len = 1;
		
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
	
	if(arglist1[0][0] == '\0')
		return NULL;
	arglist1[argnum1] = NULL;
	arglist2[argnum2] = NULL;
	
	input_redirection(arglist2);
	return arglist1;
}      

char* read_cmd(){
	bg_flag = false;
	dup2(dp_out, 1);
	dup2(dp_in, 0);
	char buff[1024];
	sprintf(buff,"\b---[%s]$ ", pwd());
	char* cmdline = (char*) malloc(sizeof(char)*MAX_LEN);
	cmdline = readline(buff);
	return cmdline;
}


bool checkForInputRedirection(char *start, int len){
	char* arg = (char*)malloc(sizeof(char)* ARGLEN);
	strncpy(arg, start, len);
	if( !strcmp(arg,"1>") ||!strcmp(arg,">") || !strcmp(arg,"0<") ||!strcmp(arg,"<"))
		return true;
	else 
		return false;

}
void input_redirection(char **arglist){
	int i = 0;
	while(arglist[i]!= NULL){
		if( !strcmp(arglist[i],"1>") ||!strcmp(arglist[i],">") ){
			dp_out = dup(1); 
			int fd_out = open(arglist[i+1], O_WRONLY | O_CREAT , 0777);
			if(fd_out == -1) perror("File Opening Error\n");
			else
				dup2(fd_out,1);
				i++;
		}
		else if( !strcmp(arglist[i],"0<") ||!strcmp(arglist[i],"<")){
			dp_in = dup(0); 
			int fd_in = open(arglist[i+1], O_RDONLY);
			if(fd_in == -1) perror("File Opening Error\n");
			else dup2(fd_in,0);
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
			if (strcmp(arglist[iter], "&") != 0 ){
				strncpy(temp_buffer[argnum], arglist[iter], sizeof(arglist[iter]));
				char* buff = strchr(temp_buffer[argnum], '$');
				if (buff == NULL);
				else {
					char* arr;
					strcpy(arr,temp_buffer[argnum]+1);
					bzero(temp_buffer[argnum],ARGLEN);
					if(getenv(arr) == NULL) printf("\n");
					else strcpy(temp_buffer[argnum],getenv(arr));
				
				}
				argnum++;	
			}
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

	if(!strcmp(arglist[0],"cd") || !strcmp(arglist[0],"jobs") || !strcmp(arglist[0],"help")|| !strcmp(arglist[0],"exit") || !strcmp(arglist[0],"history"))
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


void help(char* arglist){
	char buff[256] ;
	if(!strcmp(arglist,"cd")) printf("Change the shell working directory\n");
	else if(!strcmp(arglist,"help")) printf("Display Information about builtin commands\n");
	else if(!strcmp(arglist,"jobs")) printf("Display status of active jobs\n");
	else if(!strcmp(arglist,"exit")) printf("Exit the shell\n");	
}

void showHistory(){
	char buff[256] ;
	HISTORY_STATE *history_State = history_get_history_state();
	HIST_ENTRY **history = history_list();
	for(int i=0;i< history_State->length ;i++){
		printf("%s\n", history[i]->line);
	}
}


void executeHistory(char* cmd){
	char** arglist = (char**)malloc(sizeof(char*)* (MAXARGS+1));
	for(int j=0; j < MAXARGS+1; j++){
		arglist[j] = (char*)malloc(sizeof(char)* ARGLEN);
		bzero(arglist[j],ARGLEN);
	}
	arglist = tokenize(cmd);
	parsing_commands(arglist);
}
*/

