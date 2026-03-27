/*
 * Declares private XView interfaces, types, and macros for txt input.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_INPUT__H)
#define TXT_INPUT__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private int textsw_flush_caches(register Textsw_view_handle view, register int flags);
Pkg_private void textsw_flush_std_caches(Textsw textsw);
Pkg_private void textsw_read_only_msg(Textsw_folio textsw, int locx, int locy);
Pkg_private int textsw_note_event_shifts(register Textsw_folio textsw, register struct inputevent *ie);
Pkg_private int textsw_process_event(Textsw_view view_public, register Event *ie, Notify_arg arg);
Pkg_private Key_map_handle textsw_do_filter(register Textsw_view_handle view, register Event *ie);
#ifdef OW_I18N
Pkg_private int textsw_do_newline(register Textsw_view_handle view, int action);
#else
int textsw_do_newline(register Textsw_view_handle view, int action);
#endif
Pkg_private Es_index textsw_get_saved_insert(register Textsw_folio textsw);
Pkg_private void textsw_clear_pending_func_state(register Textsw_folio textsw);
Pkg_private void textsw_set_base_mask(Xv_object win);
Pkg_private void textsw_begin_function(Textsw_view_handle view, unsigned function);
Pkg_private void textsw_init_timer(Textsw_folio folio);
Pkg_private void textsw_end_function(Textsw_view_handle view, unsigned function);
Pkg_private void textsw_again(Textsw_view_handle view, int x, int y);
Pkg_private int textsw_end_delete(Textsw_view_handle view);
Pkg_private int textsw_function_delete(Textsw_view_handle view);
Pkg_private void textsw_undo(register Textsw_folio textsw);
#ifdef OW_I18N
Pkg_private void textsw_implicit_commit_doit(Textsw_folio folio);
#endif 

#endif
