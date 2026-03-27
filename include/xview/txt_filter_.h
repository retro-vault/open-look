/*
 * Declares public XView interfaces, types, and attributes for txt
 * filter.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_FILTER__H)
#define TXT_FILTER__H

#include <xview/pkg.h>
#include <xview_private/txt_impl.h>

Pkg_private int textsw_call_filter(register Textsw_view_handle view, char *filter_argv[]);
Pkg_private int textsw_call_smart_filter(register Textsw_view_handle view, Event *event, char *filter_argv[]);
Pkg_private void textsw_close_nonstd_fds_on_exec(void);
Pkg_private int textsw_parse_rc(Textsw_folio textsw);

#endif
