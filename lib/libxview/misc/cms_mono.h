/*
 * Declares internal interfaces used by the XView misc module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#ifndef xview_cms_mono_DEFINED
#define xview_cms_mono_DEFINED

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */
#define	CMS_MONOCHROME		"monochrome"
#define	CMS_MONOCHROMESIZE	2

#define	WHITE			0
#define	BLACK			1

#define	cms_monochromeload(r,g,b) \
	(r)[WHITE] = -1;(g)[WHITE] = -1;(b)[WHITE] = -1; \
	(r)[BLACK] = 0;(g)[BLACK] = 0;(b)[BLACK] = 0; 


#endif /* ~xview_cms_mono_DEFINED */
