/*
 * Trapezoid raster operation geometry structure definitions.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef traprop_DEFINED
#define traprop_DEFINED

struct pr_chain {
	struct pr_chain	*next;
	struct pr_size	size;		/* size of bounding box */
	int		*bits;		/* chain-encoding bits */
};

struct pr_fall {
	struct pr_pos	pos;		/* position of top of fall */
	struct pr_chain	*chain;		/* trajectory of fall */
};

struct pr_trap {
	struct pr_fall	*left, *right;	/* falls = left+right boundaries */
	int y0, y1;			/* top+bottom boundaries */
};

#endif /* traprop_DEFINED */
