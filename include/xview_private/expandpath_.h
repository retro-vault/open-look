/*
 * Declares private XView interfaces, types, and macros for expandpath.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(EXPANDPATH__H)
#define EXPANDPATH__H

void expand_path(register char *nm, register char *buf);

#endif
