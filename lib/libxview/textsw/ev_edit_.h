/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(EV_EDIT__H)
#define EV_EDIT__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview_private/ev.h>

Pkg_private unsigned ev_span(register Ev_chain views, Es_index position, Es_index *first, Es_index *last_plus_one, int group_spec);
Pkg_private Es_index ev_line_start(register Ev_handle view, register Es_index position);
Pkg_private void ev_update_lt_after_edit(register Ev_line_table *table, Es_index before_edit, register long int delta);
Pkg_private struct ei_span_result ev_span_for_edit(Ev_chain views, int edit_action);
Pkg_private int ev_delete_span(Ev_chain views, register Es_index first, register Es_index last_plus_one, Es_index *delta);
#ifdef OW_I18N
Pkg_private int ev_delete_span_bytes(Textsw_folio folio, register Es_index first, register Es_index last_plus_one, Es_index *delta);
#endif 

static void ev_update_fingers_after_edit(register Ev_finger_table *ft, register Es_index insert, register Es_index delta);

Pkg_private void ev_update_view_display(register Ev_handle view);
Pkg_private void ev_update_after_edit(register Ev_chain views, Es_index last_plus_one, Es_index delta, Es_index old_length, Es_index min_insert_pos);
Pkg_private void ev_update_chain_display(register Ev_chain views);
Pkg_private void ev_make_visible(Ev_handle view, Es_index position, int lower_context, int auto_scroll_by, Es_index delta);
Pkg_private void ev_scroll_if_old_insert_visible(register Ev_chain views, register Es_index insert_pos, register Es_index delta);
Pkg_private Es_index ev_input_partial(register Ev_chain views, CHAR *buf, int buf_len);
Pkg_private Es_index ev_input_after(register Ev_chain views, Es_index old_insert_pos, Es_index old_length);
#ifdef EXAMPLE
Pkg_private Es_index ev_process_input(register Ev_chain views, CHAR *buf, int buf_len);
#endif

#endif
