/*
 * fix for shared libraries in SunOS4.0. Code was isolated.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/p_utl_.h>

/* selection service functions */
    void            (*panel_seln_hilite_proc) () = (void (*) ()) panel_nullproc;
    void            (*panel_seln_inform_proc) () = (void (*) ()) panel_nullproc;
    void            (*panel_seln_destroy_proc) () = (void (*) ()) panel_nullproc;
