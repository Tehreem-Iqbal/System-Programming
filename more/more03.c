
//more03.c: read and print one page then pause for a few special commands ('q', ' ' , '\n')
 
#include <stdlib.h>
#include <stdio.h>
#define PAGELEN 20

int get_input()
{
   int c;		
     c=getchar();
      if(c == 'q')
	 return 0;
      else if ( c == ' ')			
	 return 1;
      else if ( c == '\n' )	
	 return 2;	
      return 3;
}

void moreV03(FILE *in){ 
	int lines_printed = 0;
	int input;
	char buff[36];
	while(fgets(buff,sizeof(buff), in) != NULL){
		fputs(buff, stdout);
		lines_printed++;
		if (lines_printed == PAGELEN){
			input =  get_input();
			if (input == 0) break;
			else if(input == 1) lines_printed-= PAGELEN; 
			else if(input == 2) lines_printed--;
			else if(input == 3) break;
		}
	 } 
}

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
		more(in);
		i++;
	}
}

