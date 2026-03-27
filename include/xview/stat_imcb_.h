/*
 * Declares public XView interfaces, types, and attributes for stat
 * imcb.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(STAT_IMCB__H)
#define STAT_IMCB__H

#include <xview/pkg.h>
#ifdef OW_I18N
#include <X11/Xlib.h>

Xv_private void _xv_status_start(XIC *ic, XPointer client_data, XIMStatusDrawCallbackStruct *cb_data);
Xv_private void _xv_status_draw(XIC *ic, XPointer client_data, XIMStatusDrawCallbackStruct *cb_data);
Xv_private void _xv_status_done(XIC *ic, XPointer client_data, XIMStatusDrawCallbackStruct *cb_data);
#endif 

#endif
