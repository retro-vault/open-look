/*
 * Declares internal interfaces used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DEMORANDOM__H)
#define DEMORANDOM__H


int xv_initrandom(int r);
int xv_random(int i, int j);
int xv_sqroot(register int a);

#endif
