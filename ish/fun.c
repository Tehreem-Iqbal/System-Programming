

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

		commandsArray = (char**) malloc(50 * sizeof(char*));
		parametersArray = (char***) malloc(50 * sizeof(char*));
		for(int j=0; j <=50; j++){
			commandsArray[j] = (char*)malloc(sizeof(char)* 50);
			bzero(commandsArray[j],50);	
		}
		for(int i=0; i <=50; i++){
			parametersArray[i] = (char**)malloc(sizeof(char)* 50);
			for (int p=0;p<=50;p++){
			parametersArray[i][p] = (char*)malloc(sizeof(char)* 50);
			bzero(parametersArray[i][p],50);				
			}
		}
			wordCount = 0; 
			commandCount = 0;
			parameterCount = 0;
			paramCount = 0;
}

void process(){
	for(int i=0;i<commandCount;i++){
		printf("-----------------");
		printf("command %d : %s\n", i, commandsArray[i]);
		for(int p=0; parametersArray[i][p] != '\0' ;p++)
			printf("command : %d , para %d : %s\n", i, p, parametersArray[i][p] );
		printf("-----------------");
	}

	for(int i=0;i<commandCount;i++){
		printf("t b decided\n");
		if(isBuiltin(commandsArray[i])) {
			printf("buiiltin\n");
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
			printf("n builtin");
			executeCommand(commandsArray[i], parametersArray[i]);
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


int executeCommand(char* command, char** args){

	char** arglist = (char**)malloc(50*sizeof(char));
	for(int j=0; j < 50; j++){
		arglist[j] = (char*)malloc(sizeof(char)* 10);
		bzero(arglist[j],10);
	}
	/*
	strcpy(arglist[0], command);
	for(int i=1;i<10;i++){
		strcpy(arglist[i], args[i-1]);
	}
	printf("%s\n", arglist[0]);
	*/
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
				waitpid(cpid, NULL, 0);
				return 0;
		}
}


