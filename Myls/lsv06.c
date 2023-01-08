/*
Add a feature that displays colorful filenames, i.e.,
directories in blue, executables in green, tarballs in red,
softlinks in pink, character and block special files in
reverse video
*/

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include<sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

#define MAXFILES 500
#define SIZE 20
extern int errno;

void do_ls(char * d);
void show_stat_info(char* fname);
char check_filetype(long s);
void check_time(long secs);
char* check_filepermissions(long s);
char* gidtogrpname(int gid);
char* uidtouname(int uid);
int comparator(const void *a, const void *b);
void print_File_Name(char fileType , char* fname);

int main(int argc, char* argv[]){
	int i;
	if (argc == 1)
		do_ls(".");  
	for(i=1; i<argc ;i++)   
		do_ls(argv[i]); 
	return 0;   
}

void do_ls(char * d){
	int iter = 0;
	char** entries = (char**)malloc(sizeof(char)*MAXFILES);
	for(int i=0;i<MAXFILES;i++)
		entries[i] = (char*)malloc(sizeof(char)*SIZE);
		
	struct stat info;
	struct dirent * entry;
	DIR * dp = opendir(d);
	int i =0, n=0;
	if (dp == NULL){
		fprintf(stderr, "No such file or directory\n");
		exit(0);
	}
	errno = 0;
	while((entry = readdir(dp)) != NULL){
		if(entry == NULL && errno != 0){
		perror("Readdir failed");
		exit(errno);
		}
		else
			strcpy(entries[iter++], entry->d_name);
	}
	
    	qsort((void*)entries, iter - 2, sizeof(entries[0]), comparator);
    
	for(int i=0;i<iter-2;i++){
		if(entries[i][0] != '.')
			show_stat_info(entries[i]);
	}
}

void show_stat_info(char* fname){
	struct stat info;
	int st_rv = stat(fname, &info);
	if (st_rv == -1){
		perror("Stat failed");
		exit(0);
	}
	
	//Filetype
	long s = info.st_mode;
	char filetype = check_filetype(s);
	
	//File Permissions
	char* permissions = check_filepermissions(s);
	
	//Link Count
	int link_Count = info.st_nlink;
	
	//Username
	int user_id = info.st_uid;
	char* username = uidtouname(user_id);
	
	//Group Name
	int grp_id = info.st_gid;
	char* group_name = gidtogrpname(grp_id);
	
	//File size
	int file_size = info.st_size;
		
	//Last modification time
	long timeInsec = info.st_mtime;
	char* mod_time = ctime(&timeInsec);
	char* ctime_no_newline = strtok(mod_time, "\n");
	
	//File name
	
	printf("%c%s %d %s %s %ld %s ", filetype, permissions, link_Count, username, group_name,file_size, mod_time);
	//check file 
	print_File_Name(filetype, fname);
}

char check_filetype(long s){
	if ((s &  0170000) == 0010000) 
		return 'p';	
	else if ((s &  0170000) == 0020000) 
		return 'c';	
	else if ((s &  0170000) == 0040000) 
		return 'd';	
	else if ((s &  0170000) == 0060000) 
		return 'b';	
	else if ((s &  0170000) == 0100000) 
		return '-';	
	else if ((s &  0170000) == 0120000) 
		return 'l';	
	else if ((s &  0170000) == 0140000) 
		return 's';		   
}

char* check_filepermissions(long s){
	char* str = (char*) malloc(sizeof(char) * 10);
	strcpy(str, "---------");
	//owner  permissions
	if((s & 0000400) == 0000400) str[0] = 'r';
	if((s & 0000200) == 0000200) str[1] = 'w';
	if((s & 0000100) == 0000100) str[2] = 'x';
	//group permissions
	if((s & 0000040) == 0000040) str[3] = 'r';
	if((s & 0000020) == 0000020) str[4] = 'w';
	if((s & 0000010) == 0000010) str[5] = 'x';
	//others  permissions
	if((s & 0000004) == 0000004) str[6] = 'r';
	if((s & 0000002) == 0000002) str[7] = 'w';
	if((s & 0000001) == 0000001) str[8] = 'x';
	//special  permissions
	if((s & 0004000) == 0004000) str[2] = 's';
	if((s & 0002000) == 0002000) str[5] = 's';
	if((s & 0001000) == 0001000) str[8] = 't';
	return str;
}

char* uidtouname(int uid){
	struct passwd * pwd = getpwuid(uid);  
	if (pwd == NULL){
	      if (errno == 0)
         	printf("Record not found in passwd file.\n");
      else
      	perror("getpwuid failed");
   	}
   	else
		return pwd->pw_name;
}


char* gidtogrpname(int gid){
	struct group * grp = getgrgid(gid);
   	errno = 0;
   	if (grp == NULL){
      	if (errno == 0)
          	printf("Record not found in /etc/group file.\n");
      else
          perror("getgrgid failed");
   	}
   	else
		return grp->gr_name;   
}
void check_time(long secs){
	char *ctime_no_newline;
	ctime_no_newline = strtok(ctime(&secs), "\n");
	printf("%s	", ctime_no_newline); 
}

int comparator(const void *a, const void *b){ 
       return strcmp(*(char **)a, *(char **)b);                              

}
void print_File_Name(char fileType , char* fname){
	if (strstr(fname, ".tar"))
		fileType = 't';

	switch(fileType){
		case 'd':
			printf("\033[35m %s \033[m \n", fname);
			break;
		case 'b':
			printf("\e[7m %s \033[m \n", fname);
			break;
		case 'c':
			printf("\e[7m %s \033[m \n", fname);
			break;
		case 't':
			printf("\033[35m %s \033[m \n", fname);
			break;
		case 'l':
			printf("\033[35m %s \033[m \n", fname);
			break;
		case '-':
			printf("\033[35m %s \033[m \n", fname);
			break;
		case 'p':
			printf("\033[35m %s \033[m \n", fname);
			break;
		case 's':
			printf("\033[35m %s \033[m \n", fname);
			break;
	}
}
