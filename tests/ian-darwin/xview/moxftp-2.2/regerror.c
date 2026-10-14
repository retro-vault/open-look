#include "regexp.h"
#include <X11/Intrinsic.h>
#include <stdio.h>

void Set_Status(char * s);

void
regerror(char * s)
{
	char *cp = 0;

 	cp = XtMalloc(strlen(s)+sizeof("regexp(3): %s  "));
	sprintf(cp, "regexp(3): %s", s);
	Set_Status(cp);
	XtFree(cp);
}
