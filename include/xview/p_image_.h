/*
 * Declares public XView interfaces, types, and attributes for p image.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_IMAGE__H)
#define P_IMAGE__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Sv1_public Pixrect *panel_button_image(Panel client_object, char *string, int width, Xv_Font font);

#endif
