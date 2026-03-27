/*
 * CG3 framebuffer pixrect private structures and operation declarations.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef	cg3var_DEFINED
#define	cg3var_DEFINED

/*
 * cg3 -- color memory frame buffer
 */

/*
 * On architectures where cg4s have been implemented we need a nice big
 * mmap offset to avoid the cg4 compatible simulated overlay/enable planes.
 * On e.g. the 386i might as well start at zero.
 */
#define CG3_MMAP_OFFSET 0

#include <pixrect/memvar.h>

/* pixrect private data */
struct cg3_data {
	struct mprp_data mprp;		/* memory pixrect simulator */
	int fd;				/* file descriptor */
};

#define	cg3_d(pr)	((struct cg3_data *) (pr)->pr_data)

/* pixrect ops vector */
extern struct pixrectops cg3_ops;

Pixrect	*cg3_make();
int cg3_destroy();
Pixrect *cg3_region();
int cg3_putcolormap();
int cg3_getcolormap();

#endif /* 	!cg3var_DEFINED */
