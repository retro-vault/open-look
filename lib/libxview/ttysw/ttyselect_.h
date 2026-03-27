/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_SELECT__H)
#define TTY_SELECT__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>

int ttysw_is_seln_nonzero(register struct ttysubwindow *ttysw, Seln_rank rank);
Pkg_private void ttysel_init_client(register Ttysw_folio ttysw);
Pkg_private void ttysel_destroy(register struct ttysubwindow *ttysw);
Pkg_private void ttysel_acquire(register struct ttysubwindow *ttysw, register Seln_rank sel_desired);
Seln_rank ttysel_mode(register struct ttysubwindow *ttysw);
Pkg_private void ttysel_make(register struct ttysubwindow *ttysw, register struct inputevent *event, int multi);
void ttysel_move(register struct ttysubwindow *ttysw, register struct inputevent *event);
Pkg_private void ttysel_adjust(register struct ttysubwindow *ttysw, struct inputevent *event, int multi, int ok_to_extend);
void ttysel_cancel(register struct ttysubwindow *ttysw, Seln_rank rank);
Pkg_private void ttynullselection(struct ttysubwindow *ttysw);
Pkg_private void ttysel_deselect(register struct ttyselection *ttysel, Seln_rank rank);
void ttyhiliteselection(register struct ttyselection *ttysel, Seln_rank rank);
Pkg_private void ttysetselection(register struct ttysubwindow *ttysw);
void ttygetselection(Ttysw_folio ttysw);
Xv_private void ttysortextents(register struct ttyselection *ttysel, struct textselpos **begin, struct textselpos **end);
void ttysel_get_selection(Ttysw *ttysw, Seln_holder *holder);
Pkg_private int ttysw_do_copy(register Ttysw_folio ttysw);
Pkg_private int ttysw_do_paste(register Ttysw_folio ttysw);
void ttysw_do_put_get(register Ttysw_folio ttysw);

#endif
