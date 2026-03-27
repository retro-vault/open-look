/*
 * 32-bit memory pixrect compatibility data and operation declarations.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef		mem32var_DEFINED
#define		mem32var_DEFINED

#include	<pixrect/pixrect.h>
#include	<pixrect/memvar.h>	/* mprp */
#include <sys/cms.h>

#define         MEM32_8BIT_CMAPSIZE     256

struct mprp32_data
{
    struct mprp_data    mprp;
    int                 plane_group;
    int                 fd;
    int                 windowfd;
    struct colormapseg  cms;
};

#define		mprp32_d(pr)		((struct mprp32_data *) (pr)->pr_data)

extern
struct pixrectops   mem32_ops;
int                 mem32_rop();
int                 mem32_getcolormap();

#endif					/* !mem32var_DEFINED */
