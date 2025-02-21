// Reading from a file line by line and output on screen
#include <stdio.h>
#include <stdlib.h>
int main(int args, char *argv[]){
	if (args <= 1){
	printf("more - bad usage\nTry 'more --help' for more information");
	exit(1);
	}
	FILE* in = fopen(argv[1], "r");
	if (in == NULL){
		printf("Cannot open file");
		exit(1);
	}
	char buff[100];
	while(fgets(buff,sizeof(buff), in) != NULL){	
		puts(buff);
	}
}



