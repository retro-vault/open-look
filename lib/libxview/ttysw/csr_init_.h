/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CSR_INIT__H)
#define CSR_INIT__H

#include <xview/pkg.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>

Pkg_private int wininit(Xv_object win, int *maximagewidth, int *maximageheight);
#ifdef OW_I18N
Pkg_private void xv_new_tty_chr_font(Xv_opaque font);
#else
Pkg_private void xv_new_tty_chr_font(Pixfont *font);
#endif

#endif
