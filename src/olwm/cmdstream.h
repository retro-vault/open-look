/*
 * cmdstream.h: declarations and shared types for the cmdstream module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef _OLWM_CMDSTREAM_H
#define _OLWM_CMDSTREAM_H
#include <stdio.h>

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

/* ----------------------------------------------------------------------
 *      Command Data Structures
 * ---------------------------------------------------------------------*/

typedef enum _AttrType {
	INT, FLOAT, STRING
} AttrType;

typedef union _AttrValue {
	int		ival;
	float		fval;
	char		*sval;
} AttrValue;

typedef struct _CmdAttr {
	char		*name;
	AttrType	type;
	AttrValue	value;
} CmdAttr;

typedef struct _Command {
	char		*keyword;
	void		(*callback)();
	int		attrLen;
	CmdAttr		*attrList;
} Command;

/* ----------------------------------------------------------------------
 *      Function Definitions
 * ---------------------------------------------------------------------*/
/* SetCmdStream: Set cmd stream. */
void SetCmdStream(FILE *instream, FILE *outstream);
/* SendCmd: Send cmd. */
int SendCmd(Command *cmd);
/* RegisterCmd: Register cmd. */
void RegisterCmd(Command *cmd);
/* GetCmd: Return cmd. */
int GetCmd(void);
/* ParseCmd: Parse cmd. */
int ParseCmd(char *line);

#endif /* _OLWM_CMDSTREAM_H */
