/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_SELSVC__H)
#define TXT_SELSVC__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

typedef struct tsfh_object {
    Textsw_view_handle view;
    Textsw_selection_handle selection;
    Seln_attribute  continued_attr;
    unsigned        flags;
    int             fill_result;
}               Tsfh_object;
typedef Tsfh_object *Tsfh_handle;

Pkg_private void textsw_clear_secondary_selection(register Textsw_folio textsw, unsigned type);
Pkg_private Seln_rank seln_rank_from_textsw_info(register int type);
Pkg_private Seln_rank textsw_acquire_seln(register Textsw_folio textsw, register Seln_rank rank);
Pkg_private unsigned textsw_determine_selection_type(register Textsw_folio textsw, int acquire);
Pkg_private void textsw_give_shelf_to_svc(register Textsw_folio folio);
Pkg_private int textsw_sync_with_seln_svc(register Textsw_folio folio);
Pkg_private int textsw_inform_seln_svc(register Textsw_folio textsw, register int function, int is_down);
Pkg_private void textsw_abort(register Textsw_folio textsw);
Pkg_private void textsw_may_win_exit(Textsw_folio textsw);
Pkg_private int textsw_prepare_buf_for_es_read(register int *to_read, register CHAR **buf, register int buf_max_len, int fixed_size);
Pkg_private int textsw_es_read(register Es_handle esh, register CHAR *buf, Es_index first, register Es_index last_plus_one);
Pkg_private int textsw_fill_selection_from_reply(register Tsfh_handle context, register Seln_request *reply);
Pkg_private int textsw_selection_from_holder(register Textsw_folio textsw, register Textsw_selection_handle selection, Seln_holder *holder, int type, int flags);
Pkg_private int textsw_func_selection_internal(register Textsw_folio textsw, register Textsw_selection_handle selection, int type, int flags);
Pkg_private int textsw_func_selection(register Textsw_folio textsw, register Textsw_selection_handle selection, int flags);
Pkg_private void textsw_init_selection_object(register Textsw_folio textsw, register Textsw_selection_handle selection, CHAR *buf, int buf_max_len, int buf_is_dynamic);
Pkg_private int textsw_is_seln_nonzero(register Textsw_folio textsw, int type);
Pkg_private Es_index textsw_stuff_selection(Textsw_view_handle view, int type);
Pkg_private int textsw_seln_svc_had_secondary(register Textsw_folio textsw);
Pkg_private Seln_response textsw_setup_function(register Textsw_folio folio, register Seln_function_buffer *function);
Pkg_private void textsw_end_selection_function(register Textsw_folio folio);
Pkg_private void textsw_seln_svc_function(Textsw first_textsw_public, register Seln_function_buffer *function);
char *textsw_base_name(char *full_name);
Pkg_private Es_index textsw_check_valid_range(register Es_handle esh, register Es_index first, Es_index *ptr_last_plus_one);
Pkg_private Seln_result textsw_seln_svc_reply(Seln_attribute attr, register Seln_replier_data *context, int max_length);

#endif
