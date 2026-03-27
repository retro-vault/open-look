/*
 * Declares internal interfaces used by the XView wmgr module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WMGR_STATE__H)
#define WMGR_STATE__H

#include <xview/pkg.h>
#include <xview/wmgr.h>

Xv_public void wmgr_changelevel(register Xv_object window, int parent, int top);

#endif
