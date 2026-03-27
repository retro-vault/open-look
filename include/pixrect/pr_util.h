/*
 * Utility macros and helper declarations for pixrect implementations.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef	pr_util_DEFINED
#define	pr_util_DEFINED

/*
 * WARNING:  This include file is obsolete and may disappear in the future.
 *
 * pr_product has been moved to pixrect.h
 * struct pr_devdata etc. has been moved to pr_impl_make.h
 * new loop macros are in pr_impl_util.h
 */

/*
 * Utilities for implementing pixrect operations.
 */
 
/*
 * Aids to handling overlapping source and destination.
 * Given the from and to pr_pos's, rop_direction tells
 * whether the rasterop is up or down and left or right,
 * encoded as the ROP_UP and ROP_LEFT bits or their absence.
 * The macros rop_is(up|down|left|right) can then be used.
 */
#define	ROP_UP		0x1
#define	ROP_LEFT	0x2

#define	rop_direction(src, so, dst, do)	\
	    (   ( (((dst).x+(do).x) < ((src).x+(so).x)) << 1) | \
	          (((dst).y+(do).y) < ((src).y+(so).y))      )
#define	rop_isleft(dir)		((dir)&ROP_LEFT)
#define	rop_isup(dir)		((dir)&ROP_UP)
#define	rop_isright(dir)	(((dir)&ROP_LEFT)==0)
#define	rop_isdown(dir)		(((dir)&ROP_UP)==0)

/*
 * Aids to producing fast loops, either unrolled or very tight:
 *
 * Cases8(n, op) produces the dense case part of a case statement
 * for the cases [n+1..n+8), repeating ``op'' 1-8 times respectively.
 *
 * Rop_slowloop(n, op) produces a loop to do ``op'' n times, in little space.
 * 
 * Rop_fastloop(n, op) produces a loop to do ``op'' n times, in little time.
 *
 * Loop_d6(label, op) produces a dbra loop to do ``op'' the number of times
 * in register d6 (second non-pointer register variable).
 *
 * Loop_d6 is only possible on a 68000 family processor, and rop_fastloop
 * generates an unrolled loop only on a 68010 (assumes other processors 
 * will have some kind of I-cache).
 */
#define	rop_slowloop(n, op) \
	{ register int _loop = (n); \
		while (--_loop >= 0) { op; } }

#define	rop_fastloop	rop_slowloop

/*
 * Alloctype(datatype) allocates a datatype structure using calloc
 * with the appropriate type cast.
 */
#define	alloctype(datatype)						\
	    (datatype *)calloc(1, sizeof (datatype))

/*
 * Pr_product is used when doing multiplications involving pixrects,
 * and casts its arguments to that the compiler will use 16 by 16 multiplies.
 */
#ifndef pr_product
#define pr_product(a, b)	((a) * (b))
#endif

/*
 * Pr_area is the area of a rectangle.
 */
#define pr_area(size) pr_product((size).x, (size).y)

/*
 * Pr_devdata is used to keep track of the valloced/mmapped virtual
 * address of a device to prevent doing it more than necessary.
 */
struct pr_devdata {
	struct pr_devdata *next; /* link to next device of this type */
	dev_t	rdev;		/* device type */
	int	count;		/* reference count */
	int	fd;		/* fd of frame buffer, -1 if unused */
	short 	*va; 		/* virtual address */
	int	bytes;		/* size of va, 0 for no munmap */
	caddr_t	va2;		/* second virtual address, 0 if unused */
	int	bytes2;		/* second size */
};

Pixrect *pr_makefromfd();

#endif /* pr_util_DEFINED */
