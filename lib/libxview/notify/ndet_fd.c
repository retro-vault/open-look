/*
 * Implements the ndet fd routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ndet_fd_.h>
#include <xview_private/ntfy_debug_.h>
#include <xview_private/ndet.h>
#if defined(SVR4) || defined(__linux__)
#include <unistd.h>
#endif /* SVR4 */

/* performance: global cache of getdtablesize() */
extern int      dtablesize_cache;
#ifdef SVR4
#define GETDTABLESIZE() \
(dtablesize_cache?dtablesize_cache:(dtablesize_cache=(int)sysconf(_SC_OPEN_MAX)))
#else
#define GETDTABLESIZE() \
    (dtablesize_cache?dtablesize_cache:(dtablesize_cache=getdtablesize()))
#endif /* SVR4 */

static int      ndet_fd_table_size;	/* Number of descriptor slots
					 * available */

pkg_private int
ndet_check_fd(fd)
    int             fd;
{
    if (ndet_fd_table_size == 0)
	ndet_fd_table_size = GETDTABLESIZE();
    if (fd < 0 || fd >= ndet_fd_table_size) {
	ntfy_set_errno(NOTIFY_BADF);
	return (-1);
    }
    return (0);
}
