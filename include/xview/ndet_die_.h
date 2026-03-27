/*
 * Declares public XView interfaces, types, and attributes for ndet
 * die.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NDET_DIE__H)
#define NDET_DIE__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>


Notify_error notify_die(Destroy_status status);
pkg_private NTFY_ENUM ndet_remove_all(NTFY_CLIENT *client, NTFY_CONDITION *condition, NTFY_ENUM_DATA context);

#endif
