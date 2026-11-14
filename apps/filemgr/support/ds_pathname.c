#ifndef lint
static  char sccsid[] = "%Zn%ds_pathname.c 3.1 92/04/03 (c) 1988 Sun Micro";
#endif

/*	Copyright (c) 1987, 1988, Sun Microsystems, Inc.  All Rights Reserved.
	Sun considers its source code as an unpublished, proprietary
	trade secret, and it is available only under strict license
	provisions.  This copyright notice is placed here only to protect
	Sun in the event the source is deemed a published work.  Dissassembly,
	decompilation, or other means of reducing the object code to human
	readable form is prohibited by the license agreement under which
	this code is provided to the user or company in possession of this
	copy.

	RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the
	Government is subject to restrictions as set forth in subparagraph
	(c)(1)(ii) of the Rights in Technical Data and Computer Software
	clause at DFARS 52.227-7013 and in similar clauses in the FAR and
	NASA FAR Supplement. */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>		/* MAXPATHLEN */
#include <string.h>
#ifndef SVR4
#include <strings.h>
#endif /* SVR4 */
#include <ctype.h>
#include <pwd.h>


/******************************************************************************
*
*       Function:       ds_expand_pathname
*
*       Description:    Expand ~'s and environment variables in a path.
*
*			This routine was stolen from filemgr. I've made a
*			couple of small changes to better handle "~user"
*			at the start of a path.
*
*       Parameters:     path	Unexpanded path
*                       bug    	Returned expanded path.  Caller is responsible
*				allocating enough space.
*
*       Returns:        Nothing
*
******************************************************************************/
void
ds_expand_pathname(path, buf)
	char *path;			/* User's unexpanded path */
	char *buf;			/* Return expanded path */
{
	register char *p, *b_p, *e_p;	/* String pointers */
	char *save_p;			/* Point in path before env var */
	char env[255];			/* Environment variable expansion */
	struct passwd *pw;		/* Password file entry */

	p = path;
	if (*p == '~')
	{
		p++;
		if (*p && *p != '/')
		{
			/* Somebody else's home directory? */
			if (b_p = strchr(p, '/'))
				*b_p = '\0';
			if (pw = getpwnam(p)) {
				(void) strcpy(buf, pw->pw_dir);
			} else {
				*buf = '~';
				(void) strcpy(buf + 1, p);
			}

			if (b_p)  {
				*b_p = '/';
				p = b_p;
			} else
				return;

		}
		else
			(void) strcpy(buf, (char *) getenv("HOME"));
		b_p = buf + strlen(buf);
	}
	else
		b_p = buf;

	while (*p)
		if (*p == '$')
		{
			/* Expand environment variable */
			save_p = p;
			e_p = env;
			p++;
			while ((isalnum(*p) || *p == '_') && e_p - env < 255)
				*e_p++ = *p++;
			*e_p = 0;
			if (e_p = (char *) getenv(env))
				while (*e_p)
					*b_p++ = *e_p++;
			else
			{
				p = save_p;
				*b_p++ = *p++;
			}
		}
		else
			*b_p++ = *p++;
	*b_p = 0;
}

