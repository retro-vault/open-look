/*
 * Implements the propsrecv routines used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

/* ----------------------------------------------------------------------
 *	propscmdrecv.c
 * ---------------------------------------------------------------------*/
#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#) propsrecv.c 1.3 93/06/28 Sun Micro";
#endif
#endif

#include "propscmd.h"

extern	int	ShowWindowProps();

/* ----------------------------------------------------------------------
 *	RegisterPropsWindow
 * ---------------------------------------------------------------------*/
void
RegisterPropsCmd()
{
	propsCommand.callback = ReceivePropsCmd;
	RegisterCmd(&propsCommand);
}

/* ----------------------------------------------------------------------
 *	ReceivePropsCmd
 * ---------------------------------------------------------------------*/
void
ReceivePropsCmd(cmd)
	Command		*cmd;
{
	CmdAttr		*attr;

	attr = cmd->attrList;
	ShowWindowProps(attr[PW_SCREEN_NO].value.ival);
}
