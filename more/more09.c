//more09: ADD FUNCTIONALITY  to open file in vim


#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>  
#include <unistd.h>
#include <pthread.h>

#define REVERSE_VIDEO printf("\033[7m");
#define TRUE_VIDEO printf("\033[m");
#define ONELINEUP printf("\033[1A");
#define ERASECURRENTLINE printf("\033[2K");
#define MOVETOCOLUMNONE printf("\033[1G");
#define ERASE_SCREEN printf("\033[1J");
void echoMode();
void non_EchoMode();
void canonMode();
void non_canonMode();
int getPageLength();
int get_input();
off_t getFileSize(FILE *fp);
int getPercentage(FILE *fp, int fileSize, int linesPrinted);
void vim(char *const filename);
void more(FILE *in, char *const filename);

int main(int args, char *argv[]){
	if (args == 1){
		printf("more - bad usage\nTry 'more --help' for more information");
		exit(1);
	}

	else{
		int i=1;
		while(i < args){
			FILE* in = fopen(argv[i], "r");
			if (in == NULL){
			printf("Cannot open file");
			exit(1);
			}
			more(in, argv[i]);
			i++;
			fclose(in);
		}
	}
	
}



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


int getPageLength(){
	struct winsize window;
	ioctl(0, TIOCGWINSZ, &window);
	return window.ws_row;
}

int get_input(){
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
	else if ( c == 'v' )
		return 3;
   	return 4;
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

void vim(char *const filename){
	pid_t ch_pid;
	char *const parmList[] = {"/usr/bin/vim", filename, NULL};
	ch_pid = fork();
	if (ch_pid == -1){
		perror("Fork error");
	}			
	else if (ch_pid == 0){ 
        	execvp("/usr/bin/vim", parmList);	
        } 
        else if(ch_pid > 0){
       		wait(NULL);
       		//return ch_pid;
        }	
        //pause();
}

void more(FILE *in, char *const filename){ 
	int fileSize = getFileSize(in);
	fseek(in, 0, SEEK_SET);
	int total_lines = 0;
	int page_length = 0;
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
			int CFO = fseek(in,0 , SEEK_CUR);
			vim(filename);
			fseek(in, CFO, SEEK_SET);
			}
			else if(input == 4) {
			ERASECURRENTLINE; MOVETOCOLUMNONE; 
			break;}
			
		}
	 } 
}






