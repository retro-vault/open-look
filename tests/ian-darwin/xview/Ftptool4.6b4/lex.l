%{

#ifndef hpux
#pragma ident   "@(#)lex.l 1.3     93/08/19"
#endif

#undef ECHO
#undef input
#include "ftptool.h"

#undef input
extern char *lex_string;
#define YY_INPUT(buf, result, max_size) \
    do { \
        (void)(max_size); \
        if (*lex_string) { (buf)[0] = *lex_string++; (result) = 1; } \
        else { (result) = YY_NULL; } \
    } while (0)
%}
%%
[ \t\n]+	{return(' ');}
PERMS		{return(PERMS);}
LINKS		{return(LINKS);}
USER		{return(USER);}
GROUP		{return(GROUP);}
SIZE		{return(SIZE);}
MONTH		{return(MONTH);}
DAY			{return(DAY);}
TIME		{return(TIME);}
NAME		{return(NAME);}
SKIP		{return(SKIP);}
NONUNIX		{return(NONUNIX);}
\<NAME\>		{return(LOWERNAME);}
.			{return(yytext[0]);}
%%

int yywrap(void)
{
	return 1;
}
