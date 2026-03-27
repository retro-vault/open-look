/*
 * Declares public XView interfaces, types, and attributes for filter.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FILTER__H)
#define FILTER__H

#include <xview/pkg.h>
#include <xview_private/filter.h>

struct filter_rec **xv_parse_filter_table(STREAM *in, char *filename);
void xv_free_filter_table(struct filter_rec **table);

#endif
