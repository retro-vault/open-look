/*
 * Declares private XView interfaces, types, and macros for linux
 * select.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if defined(__linux__) && !defined(__LINUX_SELECT_H)
#define __LINUX_SELECT_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>

int linux_select(int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *utimeout);

#ifndef __GLIBC__
int sys_fcntl(int fildes, int cmd, int arg);
int sys_read(int fildes, char * buf, off_t cnt);
#endif

#endif /* __LINUX_SELECT_H */
