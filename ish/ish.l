%Start PARAM FNAME
%{
#include "y.tab.h"
#include <string.h>

//extern char *malloc();

//YYSTYPE yylval;

%}

separator 	[&|;]
alpha 		[A-Za-z]
number 		[0-9]
others		[%_#@$.*/:-]
backspecial	[\\]([&|;<>/]|{alpha}|{number})
word		({alpha}|{number}|{backspecial}|{others})*
redirect	[<>]
jobnumber	[%][0-9]*
spaces		[ \t]
string		({word}|{spaces})*

%%
<INITIAL>{word} { 
		    int len = strlen(yytext);
		    yylval.string = malloc(len +1);
		    strncpy(yylval.string, yytext, len);
		    yylval.string[len] = '\0';
		    BEGIN(PARAM);
		    return COMMAND;
		}

<FNAME>{word} { 
		    int len = strlen(yytext);
		    yylval.string = malloc(len +1);
		    strncpy(yylval.string, yytext, len);
		    yylval.string[len] = '\0';
		    BEGIN(PARAM);
		    return FILENAME;
		}

{word}		{
		    int len = strlen(yytext);
		    yylval.string = malloc(len +1);
		    strncpy(yylval.string, yytext, len);
		    yylval.string[len] = '\0';
		    return WORD;
		}

[>][>][&]	{ 
		    BEGIN(FNAME);
		    return APPEND_ERROR; 
		}

[>][>]		{ 
		    BEGIN(FNAME);
		    return APPEND; 
		}

[|][&]		{ 
		    BEGIN(INITIAL);
		    return PIPE_ERROR; 
		}

[>][&]		{ 
		    BEGIN(FNAME);
		    return REDIRECT_ERROR; 
		}

"<"		{
		    BEGIN(FNAME);
		    return REDIRECT_IN;
		}

">"		{
		    BEGIN(FNAME);
		    return REDIRECT_OUT;
		}

"||"		{	
		    BEGIN(INITIAL);
		    return LOGICAL_OR;
		}

"&&"		{	
		    BEGIN(INITIAL);
		    return LOGICAL_AND;
		}

"&"		{	
		    BEGIN(INITIAL);
		    return BACKGROUND;
		}

"|"		{	
		    BEGIN(INITIAL);
		    return PIPE;
		}

";"		{	
		    BEGIN(INITIAL);
		    return SEMICOLON;
		}

"\'"{string}"\'" {	
		    int len = strlen(yytext);
		    yylval.string = malloc(len + 1);
		    strncpy(yylval.string, yytext, len);
		    yylval.string[len] = '\0';
		    return STRING;
		}

"\""{string}"\"" {	
		    int len = strlen(yytext);
		    yylval.string = malloc(len + 1);
		    strncpy(yylval.string, yytext, len);
		    yylval.string[len] = '\0';
		    return STRING;
		}

"-"{word}	{	
		    int len = strlen(yytext);
		    yylval.string = malloc(len + 1);
		    strncpy(yylval.string, yytext, len);
		    yylval.string[len] = '\0';
		    return OPTION;
		}

\n		{   
		    BEGIN(INITIAL);
		    return -1; 
		}

[ \t]+		;

.		{
		    printf("Invalid %s\n", yytext);
		}
