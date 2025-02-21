
//more02.c: get multiple file names via command line argument and print entire contents of all the files and exits

#include <stdlib.h>
#include <stdio.h>
int main(int args, char *argv[]){
	if (args <= 1){
	printf("more - bad usage\nTry 'more --help' for more information");
	exit(1);
	}
	
	int i = 1;
	while(i < args){
		FILE* in = fopen(argv[i], "r");
		if (in == NULL){
		printf("Cannot open file");
		exit(1);
		}
		char buff[36];
		while(fgets(buff,sizeof(buff), in) != NULL){ puts(buff);}
		i++;}
}

