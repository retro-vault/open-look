/*
 * Implements the mem routines used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef lint
#ifdef sccs
static	char	sccsid[] = "@(#) mem.c 26.1 90/08/14 Crucible";
#endif
#endif

/*
 * Safe memory allocation/free routines - front-ends the C library functions
 *
 */

#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>		/* exit() */

static void ErrorGeneral(char *txt);


void *
MemAlloc(sz)
unsigned int sz;
{
	void *p;

	if ((p = malloc(sz)) == NULL)
		ErrorGeneral("Memory allocation failure.");
	memset((char *)p, 0, (int)sz);
	return p;
}

void *
MemCalloc(num,sz)
unsigned int num;
unsigned int sz;
{
	void *p;

	if ((p = calloc(num,sz)) == NULL)
		ErrorGeneral("Memory array allocation failure.");
	memset((char *)p, 0, (int)sz*(int)num);
	return p;
}

void
MemFree(p)
void *p;
{
	if (p != NULL)
		free(p);
}

static void
ErrorGeneral(txt)
char	*txt;
{
	(void)fprintf(stderr,"olwmslave: Fatal Error: %s\n",txt);
	exit(-1);
}
