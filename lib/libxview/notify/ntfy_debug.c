/*
 * Implements the ntfy debug routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ntfy_debug_.h>
#include <xview_private/gettext_.h>
#include <xview_private/xv_.h>
#include <stdio.h>
#include <xview_private/i18n_impl.h>
#include <xview/xv_error.h>
#include <xview_private/ntfy.h>
#include <stdlib.h>			/* free(), getenv() */

pkg_private_data int ntfy_errno_no_print;
pkg_private_data int ntfy_warning_print;

pkg_private_data int ntfy_errno_abort;
pkg_private_data int ntfy_errno_abort_init;

pkg_private void
ntfy_set_errno_debug(error)
    Notify_error    error;
{
    notify_errno = error;
    if ((!ntfy_errno_no_print) && error != NOTIFY_OK)
	notify_perror("Notifier error");
    if (!ntfy_errno_abort_init) {
	char           *str = getenv("Notify_error_ABORT");

	if (str && (str[0] == 'y' || str[0] == 'Y'))
	    ntfy_errno_abort = 1;
	else
	    ntfy_errno_abort = 0;
    }
    if (ntfy_errno_abort == 1 && error != NOTIFY_OK)
	abort();
}

pkg_private void
ntfy_set_warning_debug(error)
    Notify_error    error;
{
    notify_errno = error;
    if (ntfy_warning_print && error != NOTIFY_OK)
	notify_perror("Notifier warning");
}

pkg_private void
ntfy_assert_debug(code)
    int		    code;
{
    char	   *error_string;

    error_string = xv_malloc(strlen("Notifier internal error (code #999)") + 1);
    sprintf(error_string, "Notifier internal error (code #%d)", code);
    xv_error((Xv_object)NULL,
	     ERROR_STRING, error_string,
	     NULL);
    free(error_string);
}

pkg_private void
ntfy_fatal_error(msg)
    char           *msg;
{
    char	   *error_string;

    error_string = xv_malloc(strlen(msg) + strlen(XV_MSG("Notifier fatal error: "))
			  + 2);
    strcpy(error_string, XV_MSG("Notifier fatal error: "));
    strcat(error_string, msg);
    xv_error((Xv_object)NULL,
	     ERROR_STRING, error_string,
	     NULL);
    free(error_string);
}
