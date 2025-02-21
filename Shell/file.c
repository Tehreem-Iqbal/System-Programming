#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>


#define MAXARGS 50
#define ARGLEN 30
#define HISTORY_COUNT 1024

int main(){

	char** history= (char**)malloc(sizeof(char*)* HISTORY_COUNT);
	for(int j=0; j < MAXARGS+1; j++){
		history[j] = (char*)malloc(sizeof(char)* MAXARGS);        
		bzero(history[j],MAXARGS);
	}
	
	int fd = open("history.txt", O_RDONLY);
	int k = 0;
	while(read(fd, history[k++],MAXARGS));	
	for (int i =0; i< k ;i++)
		if(strlen(history[i]) != 0)
			printf("%s\n",history[i]);
	history[k] = "ls";
	history[k+=1] = "ls1";
	history[k+=1] = "ls2";
	int fd1 = open("history.txt", O_WRONLY);
	for (int i=0;i<=k; i++){
		write(fd1, history[i], strlen(history[i]));
		write(fd1, "\n", 1);
	}		
	

}
