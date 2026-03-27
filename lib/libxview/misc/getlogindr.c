/*
 * Implements the getlogindr routines used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/getlogindr_.h>
#include <xview_private/gettext_.h>
#include <stdio.h>
#include <pwd.h>
#include <xview_private/i18n_impl.h>
#include <xview/xv_error.h>
#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>			/* getenv() */
#endif

char           *
xv_getlogindir()
{
    struct passwd  *passwdent;
    char           *home, *loginname;

    home = getenv("HOME");
    if (home != NULL)
	return (home);
    loginname = getlogin();
    if (loginname == NULL)
	passwdent = getpwuid(getuid());
    else
	passwdent = getpwnam(loginname);
    if (passwdent == NULL) {
	xv_error((Xv_object)NULL,
		 ERROR_STRING,
		     XV_MSG("xv_getlogindir: couldn't find user in password file"),
		 NULL);
	return (NULL);
    }
    if (passwdent->pw_dir == NULL) {
	xv_error((Xv_object)NULL,
		 ERROR_STRING,
		     XV_MSG("xv_getlogindir: no home directory in password file"),
		 NULL);
	return (NULL);
    }
    return (passwdent->pw_dir);
}
