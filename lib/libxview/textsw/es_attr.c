/*
 * Implements the es attr routines used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/es_attr_.h>
#include <sys/types.h>
#include <xview/attrol.h>
#include <xview/pkg.h>
#include <xview_private/primal.h>

Pkg_private int
#ifdef ANSI_FUNC_PROTO
_es_set(Es_handle esh, ...)
#else
_es_set(esh, va_alist)
    Es_handle esh;
va_dcl
#endif
{
    va_list  valist;
    AVLIST_DECL;

    VA_START( valist, esh );
    MAKE_AVLIST( valist, avlist );
    va_end( valist );
    return (esh->ops->set(esh, avlist));
}
