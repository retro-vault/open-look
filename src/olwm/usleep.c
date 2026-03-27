/*
 * usleep.c: usleep() compatibility shim.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#include <unistd.h>

int
olwm_usleep(usec)
	unsigned int usec;
{
	return usleep(usec);
}
