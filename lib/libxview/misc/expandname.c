/*
 * Implements the expandname routines used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/expandname_.h>
#include <xview_private/gettext_.h>
#include <xview_private/xv_.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#if defined(sparc) && !defined(linux)
#ifdef SVR4
#include <unistd.h>
#else
#include <vfork.h>
#endif /* SVR4 */
#endif
#ifdef __linux__
#include <unistd.h>
#endif

#ifdef SVR4
#include <sys/signal.h>
#endif /* SVR4 */
#include <xview_private/i18n_impl.h>
#include <xview_private/portable.h>
#include <xview/expandname.h>
#include <xview/base.h>
#include <xview/xv_error.h>

#define MAXCHAR 255
#define NOSTR   ((char *) 0)

#ifdef __linux__
#include <stdlib.h>
#else
char           *getenv();
#endif


static int xv_anyof(register char *s1, register char *s2);

static char    *Default_Shell = "/bin/sh";

#undef NCARGS
#define	NCARGS	10240

struct namelist *
xv_expand_name(name)
    char            name[];
{
    char            xnames[NCARGS];
    char            cmdbuf[BUFSIZ];
    register int    pid, length;
    register char  *Shell;
    int             status, pivec[2], result;
    char           *echo = "echo ";
    char           *trimchars = "\t \n";

    /* Strip off leading & trailing whitespace and cr's */
    while (XV_INDEX(trimchars, *name))
	name++;
    length = strlen(name);
    while (length && XV_INDEX(trimchars, name[length - 1]))
	length--;
    if (!length || (length + (int)strlen(echo) + 2) > BUFSIZ)
	return (NONAMES);

    (void) strcpy(cmdbuf, echo);/* init cmd */
    (void) strncat(cmdbuf, name, length);	/* copy in trimmed file name */
    cmdbuf[strlen(echo) + length] = '\0';
    name = cmdbuf + strlen(echo);	/* get trimmed file name string */

    if (!xv_anyof(name, "~{[*?$`'\"\\"))
	return (xv_mk_1list(name));

    if (pipe(pivec) < 0) {
	perror(XV_MSG("pipe"));
	return (NONAMES);
    }
    /* sprintf(cmdbuf, "echo %s", name);	 */
    if ((pid = vfork()) == 0) {
	/* sigchild();			 */
	Shell = getenv("SHELL");
	if (Shell == NOSTR)
	    Shell = Default_Shell;
	(void) close(pivec[0]);
	(void) close(1);
	result = dup(pivec[1]);
	(void) close(pivec[1]);
	(void) close(2);
	execl(Shell, Shell, "-c", cmdbuf, NULL);
	_exit(1);
    }
    if (pid == -1) {
	perror(XV_MSG("fork"));
	(void) close(pivec[0]);
	(void) close(pivec[1]);
	return (NONAMES);
    }
    (void) close(pivec[1]);
    for (status = 1, length = 0; length < NCARGS;) {
	status = read(pivec[0], xnames + length, NCARGS - length);
	if (status < 0) {
	    perror(XV_MSG("read"));
	    return (NONAMES);
	}
	if (status == 0)
	    break;
	length += status;
    }
    (void) close(pivec[0]);
#ifdef SVR4
    while (wait((union wait *) & status) != pid);
#else /* SVR4 */
    while (wait( & status) != pid);
#endif /* SVR4 */
    ;
    status &= 0377;
    if (status != 0 && status != SIGPIPE) {
	xv_error((Xv_object)NULL,
		 ERROR_STRING, XV_MSG("\"Echo\" failed"),
		 NULL);
	return (NONAMES);
    }
    if (length == 0) {
	return (xv_mk_0list());
    }
    if (length == NCARGS) {
	char            dummy[128];

	(void) sprintf(dummy, 
			XV_MSG("Buffer overflow (> %d)  expanding \"%s\""),
		       NCARGS, name);
	xv_error((Xv_object)NULL,
		 ERROR_STRING, dummy,
		 NULL);
	return (NONAMES);
    }
    xnames[length] = '\0';
    while (length > 0 && xnames[length - 1] == '\n') {
	xnames[--length] = '\0';
    }
    return (makelist(length + 1, xnames));
}


static int
xv_anyof(s1, s2)
    register char  *s1, *s2;
{
    register int    c;
    char            table[MAXCHAR + 1];

    for (c = 0; c <= MAXCHAR; c++)
	table[c] = '\0';
    while (c = *s2++)
	table[c] = 0177;
    while (c = *s1++)
	if (table[c])
	    return (1);
    return (0);
}


/*
 * Return a pointer to a dynamically allocated namelist
 * 
 * First, the 2 commonest special cases:
 */


struct namelist *
xv_mk_0list()
{
    struct namelist *result;

    result = (struct namelist *)
	xv_malloc(sizeof(int) + sizeof(char *));
    if (result == NONAMES) {
	xv_error((Xv_object)NULL,
		 ERROR_LAYER, ERROR_SYSTEM,
		 ERROR_STRING, XV_MSG("in xv_expand_name"),
		 NULL);
    } else {
	result->count = 0;
	result->names[0] = NOSTR;
    }
    return (result);
}


struct namelist *
xv_mk_1list(str)
    char           *str;
{
    struct namelist *result;

    result = (struct namelist *)
	xv_malloc((unsigned)
	       sizeof(int) + 2 * sizeof(char *) + strlen(str) +1);
    if (result == NONAMES) {
	xv_error((Xv_object)NULL,
		 ERROR_LAYER, ERROR_SYSTEM,
		 ERROR_STRING, XV_MSG("in xv_expand_name"),
		 NULL);
    } else {
	result->count = 1;
	result->names[0] = (char *) (&result->names[2]);
	result->names[1] = NOSTR;
	(void) strcpy(result->names[0], str);
    }
    return (result);
}


struct namelist *
makelist(len, str)
    register int    len;
    register char  *str;
{
    register char  *cp;
    register int    count, i;
    struct namelist *result;

    if (str[0] == '\0') {
	return (NONAMES);
    }
    for (count = 1, cp = str; cp && *cp;) {
	cp = XV_INDEX(cp, ' ');
	if (cp) {
	    count += 1;
	    *cp++ = '\0';
	}
    }
    result = (struct namelist *)
	xv_malloc((unsigned)
	       sizeof(int) + (count + 1) *sizeof(char *) + len);
    if (result == NONAMES) {
	xv_error((Xv_object)NULL,
		 ERROR_LAYER, ERROR_SYSTEM,
		 ERROR_STRING, XV_MSG("in xv_expand_name"),
		 NULL);
    } else {
	result->count = count;
	cp = (char *) (&result->names[count + 1]);
	for (i = len; i--;)
	    cp[i] = str[i];
	for (i = 0; i < count; i++) {
	    result->names[i] = cp;
	    while (*cp++);
	}
	result->names[i] = NOSTR;
    }
    return (result);
}


void
free_namelist(ptr)
    struct namelist *ptr;
{
    if (ptr)
	free((char *) ptr);
}
