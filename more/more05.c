// morev05.c: Calculate percentage and display reverse video feature and remove "MORE" line

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#define PAGELEN 20
#define REVERSE_VIDEO printf("\033[7m");
#define TRUE_VIDEO printf("\033[m");
#define ONELINEUP printf("\033[1A ");
#define ERASECURRENTLINE printf("\033[2K");
#define MOVETOCOLUMNONE printf("\033[1G");


int get_input(){	
	int c;		
     	c=getchar();  	
      	if(c == 'q')
		return 0;
      	if ( c == ' ')			
		return 1;
      	if ( c == '\n' )	
	 	return 2;	
      	return 3;
}

off_t getFileSize(FILE *fp){
	int ch;
    	int lines=0;
    	do{
        	ch = fgetc(fp);
        	if(ch == '\n') lines++;   
    	}while( ch != EOF ); 
  	return lines;
}

int getPercentage(FILE *fp, int fileSize, int linesPrinted){
	int percentage = ((float)linesPrinted/  (float)fileSize)*100;
	return percentage;
}
void more(FILE *in){ 
	int fileSize = getFileSize(in);
	fseek(in, 0L, SEEK_SET);
	int lines_printed = 0;
	int total_lines = 0;
	int input;
	char buff[36];
	while(fgets(buff,sizeof(buff), in) != NULL){
		total_lines++;
		fputs(buff, stdout);
		lines_printed++;
		if (lines_printed == PAGELEN){
			REVERSE_VIDEO;
			printf(" --More--(%d%)", getPercentage(in, fileSize, total_lines));
			TRUE_VIDEO;			
			input =  get_input();
			if (input == 0) {ONELINEUP; ERASECURRENTLINE; MOVETOCOLUMNONE;
			break;  }
			else if(input == 1) { ONELINEUP;  ERASECURRENTLINE; MOVETOCOLUMNONE;
			lines_printed-= PAGELEN; }
			else if(input == 2) { ONELINEUP; ERASECURRENTLINE; MOVETOCOLUMNONE; 
			lines_printed--; }
			else if(input == 3) { ONELINEUP; ERASECURRENTLINE; MOVETOCOLUMNONE; 
			break;}
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
		fclose(in);
	}	
	return 0;	

}

