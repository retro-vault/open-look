/*
 * Implements the sys read routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/sys_read_.h>
#ifndef SVR4
#ifndef __linux__
#include <syscall.h>
#else
#include "linux_select.h"
#endif
#else /* SVR4 */
#include <sys/syscall.h>
#endif /* SVR4 */
#include <xview_private/ntfy.h>

extern ssize_t __read(int fd, void *buf, size_t count);

pkg_private int
notify_read(fd, buf, nbytes)
    int             fd;
    char           *buf;
    int             nbytes;
{
#ifndef __linux__
    return (syscall(SYS_read, fd, buf, nbytes));
#else
#ifdef __GLIBC__
    return (__read(fd, buf, (off_t)nbytes));
#else
    return (sys_read(fd, buf, (off_t)nbytes));
#endif
#endif
}
