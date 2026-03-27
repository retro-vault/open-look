/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_DBX__H)
#define TXT_DBX__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Xv_public Textsw textsw_first(Textsw any);
Xv_public Textsw textsw_next(Textsw previous);
Pkg_private int textsw_does_index_not_show(Textsw abstract, Es_index index, int *line_index);
Xv_public int textsw_screen_line_count(Textsw abstract);
Pkg_private int textsw_screen_column_count(Textsw abstract);
Pkg_private void textsw_set_start(Textsw abstract, Textsw_index pos);
Xv_public void textsw_file_lines_visible(Textsw abstract, int *top, int *bottom);
Pkg_private void textsw_view_line_info(Textsw abstract, int *top, int *bottom);
Pkg_private int textsw_contains_line(register Textsw abstract, register int line, register Rect *rect);
Pkg_private int textsw_nop_notify(Textsw abstract, Attr_avlist attrs);
extern Textsw abstract;
extern Attr_avlist attrs;
#ifdef OW_I18N
Xv_public Textsw_index textsw_index_for_file_line_wc(Textsw abstract, int line);
#endif
Xv_public Textsw_index textsw_index_for_file_line(Textsw abstract, int line);
Pkg_private Textsw_index textsw_position_for_physical_line(Textsw abstract, int physical_line);
Xv_public void textsw_scroll_lines(Textsw abstract, int count);
Pkg_private Textsw_mark textsw_add_glyph(Textsw abstract, Textsw_index pos, Pixrect *pr, int op, int offset_x, int offset_y, int flags);
Pkg_private Textsw_mark textsw_add_glyph_on_line(Textsw abstract, int line, struct pixrect *pr, int op, int offset_x, int offset_y, int flags);
Pkg_private void textsw_remove_glyph(Textsw abstract, Textsw_mark mark, int flags);
Pkg_private void textsw_set_glyph_pr(Textsw abstract,Textsw_mark  mark, struct pixrect *pr);
Pkg_private Textsw_index textsw_start_of_display_line(Textsw abstract, Textsw_index pos);

#endif
