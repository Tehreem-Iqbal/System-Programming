
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/stat.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>

#define BACKLOG 5
#define SIZE 1000

char* getPath(char buff1[]);
char check_File_Type(char *filename);
void reaper(int sig){
   waitpid(-1, NULL, 0);
}
int main(int argc, char* argv[]){
	signal(SIGCHLD, reaper);
	char* http_response = "HTTP/1.1 200 OK\n"
	"Date: Wed, 08 Jun 2022 13:08:03 GMT\n"
	"Expires: Fri, 08 Jul 2022 13:08:03 GMT\n\n";
	
	
	int server_socket;
	int client_socket;
	
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	
	server_socket = socket(AF_INET,SOCK_STREAM, 0);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(49152);
	char *local_address = "192.168.10.13";
	inet_aton(local_address,&server_addr.sin_addr);
	memset(&(server_addr.sin_zero), '\0', sizeof server_addr.sin_zero);
	
	int bind_rv  = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	
	if (bind_rv < 0){
		perror("ERROR: Cannot bind to the local address ");
		exit(0);
	}  
	
	int listen_rv = listen(server_socket,BACKLOG);
	if (listen_rv < 0){
		perror("ERROR: Server not listening");
		exit(0);
	}  
	
	fprintf(stderr,"Server listening at http://%s:%ld...\n", local_address, ntohs(server_addr.sin_port));
		
	
	int client_len = sizeof(client_addr);
	client_socket = accept(server_socket,(struct sockaddr*)&client_addr,&client_len);
	int fork_rv = fork();
	if (fork_rv  < 0){
		perror("ERROR: Server not listening");
		exit(0);
	}
	else if(fork_rv == 0){
		fprintf(stderr,"=== Connection Established with client %s:%d ===\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port) );
		close(server_socket);
		char buff1[SIZE];

		//Reading the http Request message
		read(client_socket, buff1, sizeof(buff1));
		write(1, buff1, strlen(buff1));
		//getting URL
		char *path = getPath(buff1);
		path = path + 1;
		
		write(client_socket, http_response , strlen(http_response));
		fprintf(stderr, "\n%s\n", path);
		switch(check_File_Type(path)){
			char buff[10];
			case 'n':	
				char *err = "No such file or Directory";
				write(client_socket, err , strlen(err));
				break;
			case 'd':	
				dup2(client_socket, 1);
				sprintf(buff,"ls -l %s", path);
				system(buff);
				break;
			case 'r':
				dup2(client_socket, 1);
				if(strstr(path, ".cgi")){
					sprintf(buff,"bash %s", path);
					system(buff);	
				}
				else{
				sprintf(buff,"cat  %s", path);
				system(buff);
				}
				break;
		}
		close(client_socket);
		fprintf(stderr,"=== Connection closed with client %s:%d ===\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port) );
	}
	else if(fork_rv){
		close(client_socket);
		
	}
	
}


char* getPath(char buff1[]){

	char *inputCopy = malloc(255 * sizeof(char));
	char delim[] = " ";
	strcpy(inputCopy, buff1);
    	char * ptr = strtok(inputCopy, delim);    
    	char *path =  strtok(NULL, delim);
    	return path;
}

char check_File_Type(char *filename){
	struct stat s;
	stat(filename, &s);
	if (access (filename, F_OK) < 0 )
		return 'n';
	else if (S_ISDIR(s.st_mode))
		return 'd';
	else if (s.st_mode & S_IXUSR != 0)
		return 'x';
	else if (S_ISREG(s.st_mode))
		return 'r';
	
	
}
