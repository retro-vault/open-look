/*
 * helpcmd.h: declarations and shared types for the helpcmd module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_HELPCMD_H
#define _OLWM_HELPCMD_H

/* ----------------------------------------------------------------------
 *	helpcmd.h
 * ---------------------------------------------------------------------*/

#include "cmdstream.h"

#define		HW_SCREEN_NO		0
#define		HW_MOUSE_X		1
#define		HW_MOUSE_Y		2
#define		HW_HELPKEY		3
#define		HW_ATTR_COUNT		4

static	CmdAttr	helpCmdAttr[] = {
	{ "SCREEN_NO",	INT },
	{ "MOUSE_X",	INT },
	{ "MOUSE_Y",	INT },
	{ "HELPKEY",	STRING }
};

static Command helpCommand = {
	"SHOWHELP", 0, HW_ATTR_COUNT, helpCmdAttr
};

/* ShowHelpWindow: Show help window. */
int ShowHelpWindow(int nscreen, int mousex, int mousey, char *helpkey);

#endif /* _OLWM_HELPCMD_H */
