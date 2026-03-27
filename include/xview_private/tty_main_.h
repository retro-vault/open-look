/*
 * Declares private XView interfaces, types, and macros for tty main.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_MAIN__H)
#define TTY_MAIN__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>

Pkg_private int ttysw_pty_output_ok(register Ttysw_folio ttysw);
Pkg_private void ttysw_pty_output(register Ttysw_folio ttysw, int pty);
#ifdef OW_I18N
int ttysw_mbs_write(int pty, char *mbs, int nbytes);
#else
#ifdef DEBUG
Pkg_private void ttysw_print_debug_string(char *cp, int len);
#endif 
#endif
void ttysw_process_STI(register Ttysw_folio ttysw, register char *cp, register int cc);
Pkg_private void ttysw_pty_input(register Ttysw_folio ttysw, int pty);
Pkg_private void ttysw_consume_output(Ttysw_view_handle ttysw_view);
Pkg_private int ttysw_copy_to_input_buffer(register Ttysw_folio ttysw, CHAR *addr, register int len);
Pkg_private int ttysw_input_it(register Ttysw_folio ttysw, char *addr, register int len);
#ifdef OW_I18N
Pkg_private int ttysw_input_it_wcs(register Ttysw_folio ttysw, CHAR *addr, register int len);
#endif 
Pkg_private void ttysw_handle_itimer(register Ttysw_folio ttysw);
Xv_public int ttysw_eventstd(register Tty_view ttysw_view_public, register struct inputevent *ie);
Pkg_private void xv_tty_new_size(Ttysw_folio ttysw, int cols, int lines);
Pkg_private int ttysw_freeze(Ttysw_view_handle ttysw_view, int on);
Pkg_private void ttysw_setopt(Ttysw_folio ttysw_folio_or_view, int opt, int on);
Pkg_private int ttysw_getopt(Ttysw_folio ttysw, int opt);
Pkg_private void ttysw_flush_input(Ttysw_folio ttysw);
#ifdef OW_I18N
int wcstombs_with_null(char *mbs, wchar_t *wcs, register int nchar);
Pkg_private void ttysw_implicit_commit(Ttysw_folio folio, int as_input);
#endif

#endif
