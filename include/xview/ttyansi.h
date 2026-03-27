/*
 * Declares public XView interfaces, types, and attributes for ttyansi.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#define NOCURSOR	0
#define UNDERCURSOR	1
#define BLOCKCURSOR	2
#define LIGHTCURSOR	4


/* terminal states */
#define S_ALPHA		0
#define S_SKIPPING	1
#define S_ESCBRKT	2
#define	S_STRING	3

#define S_ESC		0x80	/* OR-ed in if an ESC char seen */

#ifndef CTRL
#define CTRL(c)		(c & 037)
#endif

#define	DEL	0x7f
#define	NUL	0
