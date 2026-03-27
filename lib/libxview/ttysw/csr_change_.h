/*
 * Declares internal interfaces used by the XView ttysw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CSR_CHANGE__H)
#define CSR_CHANGE__H

#include <xview/pkg.h>
#include <xview/selection.h>
#include <xview_private/tty_impl.h>

Pkg_private int ttysw_setboldstyle(int new_boldstyle);
Pkg_private void ttysw_set_inverse_mode(int new_inverse_mode);
Pkg_private void ttysw_set_underline_mode(int new_underline_mode);
Pkg_private int ttysw_getboldstyle(void);
int ttysw_get_inverse_mode(void);
int ttysw_get_underline_mode(void);
Pkg_private void ttysw_setleftmargin(int left_margin);
int ttysw_getleftmargin(void);
void ttysw_fixup_display_mode(char *mode);
Pkg_private void ttysw_pstring(register CHAR *s, char mode, register int col, register int row, int op);
Pkg_private void ttysw_pclearline(int fromcol, int tocol, int row);
Pkg_private void ttysw_pcopyline(int tocol, int fromcol, int count, int row);
Pkg_private void ttysw_pclearscreen(int fromrow, int torow);
Pkg_private void ttysw_pcopyscreen(int fromrow, int torow, int count);


Pkg_private void ttysw_pdisplayscreen(int dontrestorecursor);
Pkg_private void ttysw_prepair(XEvent *eventp);
Pkg_private void ttysw_drawCursor(int yChar, int xChar);


static void ttysw_displayrow(register int row, register int leftcol);
static void ttysw_paintCursor(int op);

Pkg_private void ttysw_removeCursor(void);
Pkg_private void ttysw_restoreCursor(void); 
Pkg_private void ttysw_screencomp(void); 
Pkg_private void ttysw_blinkscreen(void);
Pkg_private void ttysw_pselectionhilite(struct rect *r, Seln_rank sel_rank);
#ifdef OW_I18N
void ttysw_convert_string(CHAR *str , CHAR *ttystr);
Pkg_private int tty_character_size(CHAR c);
#endif

#endif
