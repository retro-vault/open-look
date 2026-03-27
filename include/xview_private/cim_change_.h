/*
 * Declares private XView interfaces, types, and macros for cim change.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CIM_CHANGE__H)
#define CIM_CHANGE__H

#include <xview/pkg.h>
#include <xview_private/tty_impl.h>

Pkg_private void ttysw_vpos(int row, int col);
Pkg_private void ttysw_bold_mode(void);
void ttysw_nobold_mode(void);
Pkg_private void ttysw_underscore_mode(void);
void ttysw_nounderscore_mode(void);
Pkg_private void ttysw_inverse_mode(void);
void ttysw_noinverse_mode(void);
Pkg_private void ttysw_clear_mode(void);
Pkg_private void ttysw_writePartialLine(CHAR *s, register int curscolStart);
#define JF
#ifdef JF
Pkg_private void ttysw_cim_scroll(register int n);
#else
Pkg_private void ttysw_cim_scroll(int toy, int fromy);
#endif
Pkg_private void ttysw_insert_lines(register int where, register int n);
Pkg_private void delete_lines(register int where, register int n);
void ttysw_roll(int first, int mid, int last);
void ttysw_swapregions(int a, int b, int n);
void ttysw_swap(int a, int b);
Pkg_private void ttysw_cim_clear(int a, int b);
Pkg_private void ttysw_deleteChar(int fromcol, int tocol, int row);
Pkg_private void ttysw_insertChar(int fromcol, register int tocol, int row);
#ifdef OW_I18N
Pkg_private void tty_column_wchar_type(int xChar , int yChar , int *cwidth , int *offset);
Pkg_private int tty_get_nchars(int colstart , register int colend , int row);
#endif

#endif
