

#ident "@(#)ds_template.c	3.2 - 92/12/17 Copyright 1991 Sun Microsystems, Inc"


/*
 *  Copyright (c) 1991 Sun Microsystems, Inc.
 *  All Rights Reserved.
 *
 *  Sun considers its source code as an unpublished, proprietary
 *  trade secret, and it is available only under strict license
 *  provisions.  This copyright notice is placed here only to protect
 *  Sun in the event the source is deemed a published work.  Dissassembly,
 *  decompilation, or other means of reducing the object code to human
 *  readable form is prohibited by the license agreement under which
 *  this code is provided to the user or company in possession of this
 *  copy.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the
 *  Government is subject to restrictions as set forth in subparagraph
 *  (c)(1)(ii) of the Rights in Technical Data and Computer Software
 *  clause at DFARS 52.227-7013 and in similar clauses in the FAR and
 *  NASA FAR Supplement.
 */

/*
 * This routine attempts to create a temporary file from a specified
 * template.  It will make sure that it is a unique file.  It expands
 * "%t" as the variable part of the file name.
 *
 * %t will be treated as a "small integer" that is incremented.
 * To avoid race conditions, the name is actually created to hold
 * its place.  This integer is bounded to avoid spending an insane
 * amount of time searching the directory.
 *
 * This algorithm has a major problem -- creating multiple names
 * turns into an N^2 problem.  I thought of doing some sort of
 * binary search, but the algorithm is non-trivial in the face of
 * multiple apps trying to get names at the same time.  This
 * algorithm has the benefit of utter simplicity.
 *
 * It return's a malloc'ed buffer that the calling proceedure needs
 * to free later...
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAXFILES 500	/* something that is unreasonably large */
#ifndef NULL
#define NULL 	0
#endif /*  */
static int exists_name();


/*
 * "dir" is the prefix of the pathname.  A '/' will be added between
 * "dir" and "template" if it does not already exist.
 *
 * Template is the template name.  An embedded "%t" will be expanded
 * with an integer to make a unique file name.
 *
 * mode is the file creation mode (for example, 0666)
 *
 * A string that is the concatenation of dir and template will be
 * returned if the file could be created.  This file needs to be
 * "free"d later.  If the file could not be created a null will be returned.
 */
char *
ds_expand_template(dir, template, mode)
char *dir;
char *template;
int mode;
{
	int len;
	char *base;	/* the base of the allocated name */
	char *working;	/* a pointer to the "active" area of the new name */
	char *rest;	/* the part of the template after the %t */
	char *s;
	unsigned i;
	int err;
	int havetemplate;

	len = strlen(dir) + strlen(template) + 10;

	base = malloc(len);
	if (! base) return(NULL);

	strcpy(base, dir);

	s = &base[strlen(base)];

	/* put in a directory separator, if needed */
	if (s[-1] != '/') {
		*s++ = '/';
	}


	/* find the %t in the template specification */
	havetemplate = 0;
	for (rest = template; *rest; rest++) {
		if (rest[0] == '%' && rest[1] == 't') {
			rest += 2;
			havetemplate = 1;
			break;
		}

		*s++ = *rest;
	}

	/* try the name without anything extra */
	strcpy(s, rest);
	err = exists_name(base, mode);
	if (! err) {
		return (base);
	}
	if (err != EEXIST) goto fail;	/* permission problems */

	if (! havetemplate) {
		/* there was no %t specification -- time to give up */
		goto fail;
	}

	/* advance until we find a free spot */
	for (i = 1; i < MAXFILES; i++) {
		sprintf(s, "%d%s", i, rest);

		err = exists_name(base, mode);
		if (! err) {
			return (base);
		}
		if (err != EEXIST) goto fail;
	}

	/* there are already MAXFILES of these suckers! time to give up */
fail:
	free(base);
	return (NULL);
}



/*
 * test to see if a name exists; if not then create it to avoid
 * race conditions.  Return 0 on success, or errno on failure.
 */
static int
exists_name(s, mode)
char *s;
int mode;
{
	int fd;

	fd = open(s, O_CREAT | O_EXCL, mode);

	if (fd < 0) return(errno);
	close(fd);
	return (0);
}



#ifdef TEST
main()
{
	testit ("test", "abc%t", 0000);
	testit ("test", "def%tghi", 0111);
	testit ("test", "%tjkl", 0400);
	testit ("test", "%t", 0444);
	testit ("test", "TEMP", 0644);

	exit(0);

}

testit(dir, temp, mode)
char *dir;
char *temp;
int mode;
{
	char *res;
	int i;

	for (i = 0; i < MAXFILES+10; i++) {
		res = ds_expand_template(dir, temp, mode);

		printf( "%3i %10s -> %s\n", i, temp, res ? res : "<NULL>");

		if (! res) break;
	}
}
#endif
