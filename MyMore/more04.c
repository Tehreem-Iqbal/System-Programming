
// more04.c: reverse video featureread and print one page then pause for a few special commands ('q', ' ' , '\n')

#include <stdlib.h>
#include <stdio.h>
#define PAGELEN 20
#define REVERSE_VIDEO printf("\033[7m");
#define TRUE_VIDEO printf("\033[m");
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
   //return 0;
}

void more(FILE *in){ 
	int lines_printed = 0;
	int input;
	char buff[36];
	while(fgets(buff,sizeof(buff), in) != NULL){
		//puts(buff); add new-line character default
		fputs(buff, stdout);
		lines_printed++;
		if (lines_printed == PAGELEN){
			REVERSE_VIDEO;
			printf(" --More--(%%)");
			TRUE_VIDEO;
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
