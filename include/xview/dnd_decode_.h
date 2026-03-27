/*
 * Declares public XView interfaces, types, and attributes for dnd
 * decode.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(DND_DECODE__H)
#define DND_DECODE__H

#include <xview/pkg.h>
#include <xview_private/dndimpl.h>

Xv_public Xv_opaque dnd_decode_drop(Xv_object sel_obj, Event *event);
Xv_public void dnd_done(Selection_requestor sel);

#endif
