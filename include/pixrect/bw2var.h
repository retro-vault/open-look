/*
 * BW2 monochrome framebuffer constants and operation declarations.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef	bw2var_DEFINED
#define	bw2var_DEFINED

/*
 * bw2 -- monochrome frame buffer
 */

/* standard resolution */
#define	BW2SIZEX 1152
#define	BW2SIZEY 900
#define BW2BYTES (BW2SIZEX*BW2SIZEY/8)

#define	BW2SQUARESIZEX 1024
#define	BW2SQUARESIZEY 1024
#define BW2SQUAREBYTES (BW2SQUARESIZEX*BW2SQUARESIZEY/8)

/* high resolution (bw2h) */
#define	BW2HSIZEX	1600
#define	BW2HSIZEY	1280
#define BW2HBYTES	(BW2HSIZEX*BW2HSIZEY/8)

#define	BW2HSQUARESIZEX 1440
#define	BW2HSQUARESIZEY 1440
#define BW2HSQUAREBYTES (BW2HSQUARESIZEX*BW2HSQUARESIZEY/8)


extern	struct pixrectops bw2_ops;

struct	pixrect *bw2_make();
int	bw2_destroy();

#endif	/* bw2var_DEFINED */
