/*
 * Declares the helpcmd interfaces used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

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

void RegisterHelpCmd(void);
