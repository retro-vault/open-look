/*
 * Declares the propscmd interfaces used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include "cmdstream.h"

#define		PW_SCREEN_NO		0
#define		PW_ATTR_COUNT		1

static	CmdAttr	propsCmdAttr[] = {
	{ "SCREEN_NO",	INT },
};

static Command propsCommand = {
	"SHOWPROPS", 0, PW_ATTR_COUNT, propsCmdAttr
};

void RegisterPropsCmd(void);
void ReceivePropsCmd(Command *cmd);
