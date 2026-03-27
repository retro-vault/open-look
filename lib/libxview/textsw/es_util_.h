/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ES_UTIL__H)
#define ES_UTIL__H

#include <xview/pkg.h>
#include <xview_private/es.h>

Pkg_private Es_index es_bounds_of_gap(register Es_handle esh, Es_index around, Es_index *last_plus_one, int flags);
Pkg_private int es_advance_buf(Es_buf_handle esbuf);
Pkg_private Es_index es_backup_buf(Es_buf_handle esbuf);
Pkg_private int es_make_buf_include_index(register Es_buf_handle esbuf, Es_index index, int desired_prior_count);
Pkg_private Es_status es_copy(register Es_handle from, register Es_handle to, int newline_must_terminate);

#endif
