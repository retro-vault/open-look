/*
 * Declares private XView interfaces, types, and macros for txt file.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_FILE__H)
#define TXT_FILE__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>


Pkg_private Es_handle textsw_create_ps(Textsw_folio folio, register Es_handle original, register Es_handle scratch, Es_status *status);
Pkg_private Es_handle textsw_create_file_ps(Textsw_folio folio, CHAR *name, CHAR *scratch_name, Es_status *status);
Pkg_private Es_status textsw_load_file_internal(register Textsw_folio textsw, CHAR *name, CHAR *scratch_name, Es_handle *piece_esh, Es_index start_at, int flags);
Pkg_private void textsw_destroy_esh(register Textsw_folio folio, register Es_handle esh);
Pkg_private void textsw_replace_esh(register Textsw_folio textsw, Es_handle new_esh);
Pkg_private Es_handle textsw_create_mem_ps(Textsw_folio folio, register Es_handle original);
Pkg_private int textsw_load_selection(register Textsw_folio folio, register int locx, register int locy, int no_cd);
Pkg_private CHAR *textsw_full_pathname(register CHAR *name);
Pkg_private int textsw_format_load_error(char *msg, Es_status status, CHAR *filename, CHAR *scratch_name);
Pkg_private void textsw_format_load_error_quietly(char *msg, Es_status status, CHAR *filename, CHAR *scratch_name);
Pkg_private int textsw_load_file(Textsw abstract, CHAR *filename, int reset_views, int locx, int locy);
Pkg_private int textsw_load_file_quietly(Textsw abstract, CHAR *filename, char *feedback, int reset_views, int locx, int locy);
Pkg_private Es_status textsw_process_save_error(Textsw abstract, char *error_buf, Es_status status, int locx, int locy);
Xv_public unsigned textsw_save(Textsw abstract, int locx, int locy);
Pkg_private void textsw_cd(Textsw_folio textsw, int locx, int locy);
Pkg_private Textsw_status textsw_get_from_file(Textsw_view_handle view, CHAR *filename, int print_error_msg);
Pkg_private void textsw_file_stuff(Textsw_view_handle view, int locx, int locy);
Pkg_private Textsw_status textsw_file_stuff_from_str(Textsw_view_handle view, CHAR *buf, int locx, int locy);
Pkg_private Es_status textsw_store_init(Textsw_folio textsw, CHAR *filename);
Pkg_private Es_status textsw_process_store_error(Textsw_folio textsw, CHAR *filename, Es_status status, int locx, int locy);
Xv_public unsigned textsw_store_file(Textsw abstract, CHAR *filename, int locx, int locy);
#ifdef OW_I18N
Xv_public unsigned textsw_store_file_wcs(Textsw abstract, CHAR *filename, int locx, int locy);
#endif 
Pkg_private Es_status textsw_store_to_selection(Textsw_folio textsw, int locx, int locy);
Pkg_private void textsw_reset_2(Textsw abstract, int locx, int locy, int preserve_memory, int cmd_is_undo_all_edit);
Xv_public void textsw_reset(Textsw abstract, int locx, int locy);
Pkg_private int textsw_filename_is_all_blanks(CHAR *filename);
Pkg_private int textsw_expand_filename_quietly(Textsw_folio textsw, CHAR *buf, char *err_buf, int locx, int locy);
#ifdef OW_I18N
Pkg_private int textsw_expand_filename(Textsw_folio textsw, char *buf, int buf_len, int locx, int locy);
#else
Pkg_private int textsw_expand_filename(Textsw_folio textsw, char *buf, int locx, int locy);
#endif 
Pkg_private int textsw_get_selection_as_filename(Textsw_folio textsw, CHAR *buf, int sizeof_buf, int locx, int locy);
Pkg_private void textsw_possibly_edited_now_notify(Textsw_folio folio);
Pkg_private Es_index textsw_has_been_modified(Textsw abstract);
Pkg_private int textsw_file_name(Textsw_folio textsw, CHAR **name);
#ifdef OW_I18N
Xv_public int textsw_append_file_name_wcs(Textsw abstract, CHAR *name);
#endif
Xv_public int textsw_append_file_name(Textsw abstract, CHAR *name);
Pkg_private void textsw_post_error(Textsw_opaque folio_or_view, int locx, int locy, char *msg1, char *msg2);
#ifdef OW_I18N
Pkg_private int textsw_change_directory(Textsw_folio textsw, char *filename_wc, int might_not_be_dir, int locx, int locy);
#else
Pkg_private int textsw_change_directory(Textsw_folio textsw, char *filename, int might_not_be_dir, int locx, int locy);
#endif 
#ifdef OW_I18N
Pkg_private int textsw_change_directory_quietly(Textsw_folio textsw, CHAR *filename_wc, char *err_msgs, int might_not_be_dir, int locx, int locy);
#else
Pkg_private int textsw_change_directory_quietly(Textsw_folio textsw, char *filename, char *err_msgs, int might_not_be_dir, int locx, int locy);
#endif 
#ifndef SVR4
Pkg_private Es_status textsw_checkpoint_internal(Textsw_folio folio);
#endif 
Pkg_private Es_status textsw_checkpoint(Textsw_folio folio);
Pkg_private int textsw_empty_document(Textsw abstract, Event *ie); 
Pkg_private void textsw_post_need_selection(Textsw abstract, Event *ie);
Pkg_private void textsw_load_file_as_menu(Textsw abstract, Event *ie);
Pkg_private int textsw_handle_esc_accelerators(Textsw abstract, Event *ie);
#ifdef OW_I18N
Pkg_private void textsw_invalid_data_notice(Textsw_view_handle view, CHAR *filename, int flag);
#endif 

#endif
