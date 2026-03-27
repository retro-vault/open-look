/*
 * Implements the ei attr routines used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/ei_attr_.h>
#include <sys/types.h>
#include <xview/attrol.h>
#include <xview/pkg.h>
#include <xview_private/primal.h>

Pkg_private int
#ifdef ANSI_FUNC_PROTO
_ei_set(Ei_handle eih, ...)
#else
_ei_set(eih, va_alist)
    register Ei_handle eih;
va_dcl
#endif
{
    va_list  valist;
    AVLIST_DECL;

    VA_START( valist, eih );
    MAKE_AVLIST( valist, avlist );
    va_end( valist );
    return (eih->ops->set(eih, avlist));
}
