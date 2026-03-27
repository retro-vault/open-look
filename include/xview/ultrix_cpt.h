/*
 * This file provides a limited compatibility with older BSD variants.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef xview_ultrix_compat_DEFINED
#define xview_ultrix_compat_DEFINED

#ifdef OLD_BSD_FDSETS

#ifndef NBBY
#define NBBY    8               /* number of bits in a byte */
#endif

#ifndef NFDBITS
#define	NFDBITS	(sizeof(int) * NBBY)
#define I_DEFINED_NFDBITS	/* register the fact that I defined this */
#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE	NFDBITS
#define I_DEFINED_FDSETSIZE	/* register the fact that I defined this */
#endif

/*
 *	Here we assume that the only use of howmany(x, y) is 
 *	howmany(FD_SETSIZE, NFDBITS). If we defined both FD_SETSIZE and
 * 	NFDBITS, then we already know what howmany(x, y) will be: 1.
 *	If we did not define FD_SETSIZE and NFDBITS, then we'll have
 *	to calculate the value of howmany(x, y).
 */

#if defined(I_DEFINED_FDSETSIZE) && defined(I_DEFINED_NFDBITS)
#define howmany(x, y)	1
#else
#define howmany(x, y)	(((x)+((y)-1))/(y))
#endif

#define FD_SET(n, p)    ((p)->fds_bits[0] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)    ((p)->fds_bits[0] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  ((p)->fds_bits[0] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)      ((p)->fds_bits[0] = 0)

#endif /* OLD_BSD_FDSETS */

#endif /* ~xview_ultrix_compat_DEFINED */
