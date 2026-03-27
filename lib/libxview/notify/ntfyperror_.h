/*
 * Declares internal interfaces used by the XView notify module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NTFYPERROR__H)
#define NTFYPERROR__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

void notify_perror(char *str);

#endif
