/*
 * fix for shared libraries in SunOS4.0. Code was isolated.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview/pkg.h>
#include <xview_private/ntfy.h>

struct itimerval NOTIFY_NO_ITIMER = {{0, 0}, {0, 0}};
struct itimerval NOTIFY_POLLING_ITIMER = {{0, 1}, {0, 1}};
