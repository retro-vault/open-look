/*
 * Implements the helprecv routines used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

/* ----------------------------------------------------------------------
 *	helpcmdrecv.c
 * ---------------------------------------------------------------------*/
#ifndef line
#ifdef sccs
static	char	sccsid[] = "@(#) helprecv.c 1.3 93/06/28 Sun Micro";
#endif
#endif

#include "helpcmd.h"

extern	int	ShowHelpWindow();
static	void	ReceiveHelpCmd();

/* ----------------------------------------------------------------------
 *	RegisterHelpWindow
 * ---------------------------------------------------------------------*/
void
RegisterHelpCmd()
{
	helpCommand.callback = ReceiveHelpCmd;
	RegisterCmd(&helpCommand);
}

/* ----------------------------------------------------------------------
 *	ReceiveHelpCmd
 * ---------------------------------------------------------------------*/
static void
ReceiveHelpCmd(cmd)
	Command		*cmd;
{
	CmdAttr		*attr;

	attr = cmd->attrList;
	ShowHelpWindow(	attr[HW_SCREEN_NO].value.ival,
			attr[HW_MOUSE_X].value.ival,
			attr[HW_MOUSE_Y].value.ival,
			attr[HW_HELPKEY].value.sval
		      );
}
