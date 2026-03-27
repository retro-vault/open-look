/*
 * Declares internal interfaces used by the XView icon module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ICON_INPUT__H)
#define ICON_INPUT__H

#include <xview/pkg.h>
#include <xview/icon.h>


Notify_value icon_input(Icon icon_public, Event *event, Notify_arg arg, Notify_event_type type);

#endif
