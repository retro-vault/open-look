/*
 * slave.c: implementation of the slave module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <sys/types.h>
#include <sys/time.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>		/* exit() */

#include "cmdstream.h"
#include "error.h"


/* ----------------------------------------------------------------------
 *	Local Data
 * ---------------------------------------------------------------------*/
 
typedef struct {
	char	*program;
	pid_t	pid;
} SlaveInfo;

static SlaveInfo slaveInfo = {
	"olwmslave", 0
};


/* ----------------------------------------------------------------------
 *	Local Forward Declarations
 * ---------------------------------------------------------------------*/

static void	SlaveFailure(void);


/* ----------------------------------------------------------------------
 *	SlaveStart
 * ---------------------------------------------------------------------*/
 
pid_t
SlaveStart(argv)
	char		**argv;
{
	int		input[2],output[2];
	int		fd,maxfd;

	if (pipe(input) == -1) {
		ErrorWarning("Couldn't create input pipe for olwmslave");
		perror("pipe");
		SlaveFailure();
		return -1;
	}
	if (pipe(output) == -1) {
		ErrorWarning("Couldn't create output pipe for olwmslave");
		perror("pipe");
		SlaveFailure();
		(void)close(input[0]);
		(void)close(input[1]);
		return -1;
	}

	slaveInfo.pid = fork();

	switch (slaveInfo.pid) {
	case -1:			/* error */
		(void)close(input[0]);
		(void)close(input[1]);
		(void)close(output[0]);
		(void)close(output[1]);
		SlaveFailure();
		break;
	case 0:				/* Slave */
		dup2(input[0],0);
		dup2(output[1],1);
		maxfd = sysconf(_SC_OPEN_MAX);
		for (fd=3; fd<maxfd ; fd++) {
			(void)close(fd);
		}
		argv[0] = slaveInfo.program;
		if (execvp(slaveInfo.program,argv) == -1) {
			SlaveFailure();
			exit(-1);
		}
		break;
	default:			/* parent */
		SetCmdStream(fdopen(output[0],"r"), 	/* reader */
			     fdopen(input[1],"w"));	/* writer */
		(void)fcntl(output[0],F_SETFD,1);
		(void)fcntl(input[1],F_SETFD,1);
		close(output[1]);
		close(input[0]);
		break;
	}
	return slaveInfo.pid;
}

/* ----------------------------------------------------------------------
 *	SlaveStop	- call to stop Slave process
 * ---------------------------------------------------------------------*/
void
SlaveStop()
{
	if (slaveInfo.pid == 0)
		return;
	if (kill(slaveInfo.pid,SIGTERM) == -1) {
		/* -- dont really want this error message
		ErrorWarning("Couldn't kill olwmslave");
		perror("kill");
		*/
	}
}

/* ----------------------------------------------------------------------
 *	SlaveStopped	- called when Slave process has died
 * ---------------------------------------------------------------------*/
void
SlaveStopped()
{
	SetCmdStream((FILE *)0,(FILE *)0);
}

/* ----------------------------------------------------------------------
 *	SlaveFailure	- called if Slave process wont/cant start
 * ---------------------------------------------------------------------*/
static void
SlaveFailure()
{
	char	errbuf[256];

	sprintf(errbuf,
		"Couldn't start %s; Help for olwm will not work.",
		slaveInfo.program);
	ErrorWarning(errbuf);
}
