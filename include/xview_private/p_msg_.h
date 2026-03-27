/*
 * Declares private XView interfaces, types, and macros for p msg.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_MSG__H)
#define P_MSG__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Pkg_private int panel_message_init(Panel panel_public, Panel_item item_public, Attr_avlist avlist);

static void msg_accept_preview(Panel_item item_public, Event *event);
static void msg_paint(Panel_item item_public);

#endif
