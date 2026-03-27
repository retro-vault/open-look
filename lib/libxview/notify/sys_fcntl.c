/*
 * Implements the sys fcntl routines used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/sys_fcntl_.h>
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


extern int __fcntl(int fd, int cmd, ... /* arg */ );

pkg_private int
notify_fcntl(fd, cmd, arg)
    int             fd, cmd, arg;
{
#ifndef __linux__
    return (syscall(SYS_fcntl, fd, cmd, arg));
#else
#ifdef __GLIBC__
    return (__fcntl(fd, cmd, arg));
#else
    return (sys_fcntl(fd, cmd, arg));
#endif
#endif
}
