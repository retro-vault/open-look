/*
 * Declares internal interfaces used by the XView attr module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ATTR_CU__H)
#define ATTR_CU__H

#include <xview/pkg.h>
#include <xview/font.h>

#ifdef lint
int attr_cu_to_y(u_int encoded_value, Xv_font font, int top_margin, int row_gap);
int attr_cu_to_x(u_int encoded_value, Xv_font font, int left_margin);
void attr_replace_cu(register Attr_avlist avlist, Xv_font font, int left_margin, int top_margin, int row_gap);
#endif
int attr_rc_unit_to_y(u_int encoded_value, int row_height, int top_margin, int row_gap);
int attr_rc_unit_to_x(u_int encoded_value, int col_width, int left_margin, int col_gap);
void attr_rc_units_to_pixels(register Attr_avlist avlist, int col_width, int row_height, int left_margin, int top_margin, int col_gap, int row_gap);

#endif
