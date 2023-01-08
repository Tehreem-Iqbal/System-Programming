// more07.c: FUNCTIONALITY to compute the page size and display the file content accordingly

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>

#define PAGELEN 20
#define REVERSE_VIDEO printf("\033[7m");
#define TRUE_VIDEO printf("\033[m");
#define ONELINEUP printf("\033[1A");
#define ERASECURRENTLINE printf("\033[2K");
#define MOVETOCOLUMNONE printf("\033[1G");
#define ERASE_SCREEN printf("\033[1J");


void echoMode(){
	struct termios info;
	tcgetattr(0, &info);
	info.c_lflag = info.c_lflag | ECHO;
	tcsetattr(0, TCSANOW, &info);
}
void non_EchoMode(){
	struct termios info;
	tcgetattr(0, &info);
	info.c_lflag = info.c_lflag & ~ECHO;
	tcsetattr(0, TCSANOW, &info);
}

void canonMode(){
	struct termios info;
	tcgetattr(0, &info);
	info.c_lflag = info.c_lflag | ICANON;
	tcsetattr(0, TCSANOW, &info);
}
void non_canonMode(){
	struct termios info;
	tcgetattr(0, &info);
	info.c_lflag = info.c_lflag & ~ICANON;
	tcsetattr(0, TCSANOW, &info);
}


int get_input()
{
	int c;	
	non_EchoMode();
	non_canonMode();		
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

int getPageLength(){
	struct winsize window;
	ioctl(0, TIOCGWINSZ, &window);
	return window.ws_row;
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
	int page_length = 0;
	int total_lines = 0;	
	int lines_printed = 0;
	int input;
	char buff[36];
	while(fgets(buff,sizeof(buff), in) != NULL){
		total_lines++;
		fputs(buff, stdout);
		lines_printed++;
		page_length = getPageLength();
		if (lines_printed == page_length){
			REVERSE_VIDEO;
			printf(" --More--(%d%)", getPercentage(in, fileSize, total_lines));
			TRUE_VIDEO;
			input =  get_input();
			echoMode(); 
			canonMode();
			if (input == 0) { 
			ERASECURRENTLINE; MOVETOCOLUMNONE;	
			break;  }
			else if(input == 1) {
			ERASECURRENTLINE; MOVETOCOLUMNONE;
			lines_printed-= page_length; }
			else if(input == 2) {
			ERASECURRENTLINE; MOVETOCOLUMNONE;
			lines_printed--; }
			else if(input == 3) {
			ERASECURRENTLINE; MOVETOCOLUMNONE; 
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
	
}

.t










