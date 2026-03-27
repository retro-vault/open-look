/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTYANSI__H)
#define TTYANSI__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>

Pkg_private void ttysw_ansiinit(struct ttysubwindow *ttysw);
Pkg_private int ttysw_ansi_string(caddr_t data, char type, char c);
void ttysw_save_state(void);
void ttysw_restore_state(void);
Xv_public int ttysw_output(Tty ttysw_public, char *addr, int len0);
#ifdef OW_I18N
Xv_public int ttysw_output_wcs(Tty ttysw_public, char *addr, int len0);
#endif
Pkg_private int ttysw_output_it(Ttysw_view_handle ttysw_view, register CHAR *addr, int len0);
Pkg_private void ttysw_lighten_cursor(void);
Pkg_private void ttysw_restore_cursor(void);
Pkg_private int ttysw_ansi_escape(Tty_view ttysw_view_public, register CHAR c, register int ac, register int *av);
Pkg_private void ttysw_pos(register int col, register int row);
Pkg_private void ttysw_clear(Ttysw *ttysw);

#endif
