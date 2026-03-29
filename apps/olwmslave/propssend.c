/*
 * Implements the propssend routines used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

/* ----------------------------------------------------------------------
 *	propscmdsend.c
 * ---------------------------------------------------------------------*/
#ifndef line
#ifdef sccs
static char     sccsid[] = "@(#) propssend.c 1.3 93/06/28 Sun Micro";
#endif
#endif

#include "propscmd.h"

/* ----------------------------------------------------------------------
 *	ShowPropsWindow
 * ---------------------------------------------------------------------*/
int
ShowPropsWindow(nscreen)
	int	nscreen;
{
	propsCmdAttr[PW_SCREEN_NO].value.ival 	= nscreen;
	return(SendCmd(&propsCommand));
}
