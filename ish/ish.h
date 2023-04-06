#ifndef SHELL_H
#define SHELL_H
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <glob.h>
#include <pwd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TRUE  1
#define FALSE 0

#define OK      1
#define ERRORS  0

#define ALIAS_DEF       1
#define CDHOME_DEF      2
#define CDPATH_DEF      3
#define UNALIAS_DEF     4
#define SETENV_DEF      5
#define PRINTENV_DEF    6
#define UNSETENV_DEF    7
#define ALIASPRINT_DEF  8
#define PATH 50
#define COMMANDS 10
#define PARAMETERS 10
#define ARGLEN 10

//Functions
char* hostname();
int getCommand();
void ish_init();
void process();
bool isBuiltin(char*);
int cd(char*);
void bye();
void printEnv();
void setEnv(char*, char*);
void unsetEnv(char*);
void executeBuiltin(char* );
int executeCommand(int,char*, char**);
void destroy();


char ***parametersArray;
char** arglist;

// array that holds reach token
char** wordArray; 
char **commandsArray;

int counts[10];
// number of entries in array
int wordCount ;
int commandCount ;
int parameterCount ;
int paramCount ;

// points to an array of strings called the `environment'.


struct passwd* pwd; //contains result of getpwnam

char* HOME;
extern char **environ;
// selector
int  builtin_type;
// for builtin functions
char* cdPath;
char* variable;
char* word;

bool bg ;

// number of aliases
int   aliasCount; 
#endif
