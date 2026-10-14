#ifdef IDENT
#ident	"@(#)cmdstream.c	26.9	93/06/28 SMI"
#endif

/*
 *      (c) Copyright 1989 Sun Microsystems, Inc.
 */

/*
 *      Sun design patents pending in the U.S. and foreign countries. See
 *      LEGAL_NOTICE file for terms of the license.
 */

/* ----------------------------------------------------------------------
 *      cmdstream.c
 * ---------------------------------------------------------------------*/
#include <stdlib.h>

#include <stdio.h>
#ifdef SYSV
#include <string.h>
#else
#include <strings.h>
#endif
#include "cmdstream.h"
#include "mem.h"

extern	char	*strtok();

/* ----------------------------------------------------------------------
 *      Local Data Structures
 * ---------------------------------------------------------------------*/

typedef struct _CList {
	Command		*this;
	struct _CList	*next;
} CList;

typedef struct _CmdInfo {
	FILE		*input;
	FILE		*output;
	CList		*cmdListHead;
} CmdInfo;

/* ----------------------------------------------------------------------
 *      Data Definitions
 * ---------------------------------------------------------------------*/

static	CmdInfo		cmdInfo = {
	(FILE *)0, (FILE *)0, (CList *)0
};

/* ----------------------------------------------------------------------
 *      Local Forward Declarations
 * ---------------------------------------------------------------------*/

static Command	*MatchCommand();
static CmdAttr	*MatchAttr();
static int	EncodeAttrValue();
static int	DecodeAttrValue();

/* ----------------------------------------------------------------------
 *      SetCmdStream
 * ---------------------------------------------------------------------*/
void
SetCmdStream(FILE		* instream, FILE		* outstream)
{
	if (instream == 0)
		fclose(cmdInfo.input);
	cmdInfo.input = instream;
	if (outstream == 0)
		fclose(cmdInfo.output);
	cmdInfo.output = outstream;
}

/* ----------------------------------------------------------------------
 *      SendCmd
 * ---------------------------------------------------------------------*/
int
SendCmd(Command		* cmd)
{
	CmdAttr		*attr;
	int		i;

	if (cmdInfo.output == (FILE *)0)
		return FALSE;

	fprintf(cmdInfo.output,"%s\n",cmd->keyword);
	for (i=0; i<cmd->attrLen; i++) {
		attr = &(cmd->attrList[i]);
		fprintf(cmdInfo.output,"\t%s=",attr->name);
		EncodeAttrValue(attr,cmdInfo.output);
		fputs("\n",cmdInfo.output);
	}
	fputs(";\n",cmdInfo.output);
	fflush(cmdInfo.output);
	return TRUE;
}

/* ----------------------------------------------------------------------
 *      RegisterCmd
 * ---------------------------------------------------------------------*/
void
RegisterCmd(Command		* cmd)
{
	CList		*item;

	item = MemNew(CList);
	item->this = cmd;
	item->next = cmdInfo.cmdListHead;
	cmdInfo.cmdListHead = item;
}

/* ----------------------------------------------------------------------
 *      GetCmd
 * ---------------------------------------------------------------------*/
int
GetCmd(void)
{
#define	CMDBUFLEN	1024
	char		cmdBuf[CMDBUFLEN];

	while (fgets(cmdBuf,CMDBUFLEN,cmdInfo.input) != (char *)0) {
		if (ParseCmd(cmdBuf) == FALSE)
			return FALSE;
	}
	return TRUE;
}

/* ----------------------------------------------------------------------
 *      ParseCmd
 * ---------------------------------------------------------------------*/
int
ParseCmd(char		* line)
{
	char		*token;
	char		*sep = " \t\n";
	static Command	*cmd = (Command *)0;
	static CmdAttr	*attr = (CmdAttr *)0;

	for (token=strtok(line,sep); token; token=strtok(0,sep)) { 
		if (cmd == (Command *)0) {
			if ((cmd = MatchCommand(token)) == (Command *)0)
				return FALSE;
		} else if (token[0] == ';') {
			if (cmd->callback)
				(*cmd->callback)(cmd);
			cmd = (Command *)0;
			attr = (CmdAttr *)0;
		} else {
			if ((attr = MatchAttr(cmd,token)) == (CmdAttr *)0) {
				cmd = (Command *)0;
				return FALSE;
			}
		}
	}
#ifdef lint
	attr = attr;
#endif
	return FALSE;
}

/* ----------------------------------------------------------------------
 *      MatchKeyword
 * ---------------------------------------------------------------------*/
static Command *
MatchCommand(char		* keyword)
{
	CList		*item;

	for (item=cmdInfo.cmdListHead; item; item=item->next) {
		if (!strcmp(keyword,item->this->keyword)) {
			return item->this;
		}
	}
	return (Command *)0;
}

/* ----------------------------------------------------------------------
 *      MatchAttr
 * ---------------------------------------------------------------------*/
static CmdAttr *
MatchAttr(Command		* cmd, char		* name)
{
	int		i,equalindex;
	char		*equalpos;

	equalpos = strchr(name,'=');

	if (!equalpos)
		return (CmdAttr *)0;

	equalindex = equalpos - name;

	for (i=0; i<cmd->attrLen; i++) {
		if (!strncmp(name,cmd->attrList[i].name,equalindex)) {
			if (!DecodeAttrValue(&cmd->attrList[i],++equalpos))
				return (CmdAttr *)0;
			return &(cmd->attrList[i]);
		}
	}
	return (CmdAttr *)0;
}

/* ----------------------------------------------------------------------
 *      EncodeAttrValue
 * ---------------------------------------------------------------------*/
static int
EncodeAttrValue(CmdAttr		* attr, FILE		* stream)
{
	switch (attr->type) {
	case INT:	fprintf(stream,"%d",attr->value.ival);
			break;
	case FLOAT:	fprintf(stream,"%f",attr->value.fval);
			break;
	case STRING:	fprintf(stream,"%s",attr->value.sval);
			break;
	}
	return TRUE;
}

/* ----------------------------------------------------------------------
 *      DecodeAttrValue
 * ---------------------------------------------------------------------*/
static int
DecodeAttrValue(CmdAttr		* attr, char		* valuestr)
{
	char		*ptr;
	int		status = TRUE;

	switch (attr->type) {
	case INT:	attr->value.ival = (int)strtol(valuestr,&ptr,10);
			if (ptr == valuestr)
				status = FALSE;
			break;
	case FLOAT:	 attr->value.fval = (float)strtod(valuestr,&ptr);
			if (ptr == valuestr)
				status = FALSE;
			break;
	case STRING:	attr->value.sval = MemNewString(valuestr);
			if (attr->value.sval == (char *)0)
				status = FALSE;
			break;
	default:
			status = FALSE;
			break;
	}
	return status;
}
