#ident	"@(#)cmdstream.h	26.9	93/06/28 SMI"

/*
 *      (c) Copyright 1989 Sun Microsystems, Inc.
 */

/*
 *      Sun design patents pending in the U.S. and foreign countries. See
 *      LEGAL_NOTICE file for terms of the license.
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
void SetCmdStream(FILE *instream, FILE *outstream);
int SendCmd(Command *cmd);
void RegisterCmd(Command *cmd);
int GetCmd(void);
int ParseCmd(char *line);


#endif /* _OLWM_CMDSTREAM_H */
