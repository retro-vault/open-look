/*
 * Declares internal interfaces used by the XView file_chooser module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FC_LAYOUT__H)
#define FC_LAYOUT__H

#include <xview/pkg.h>
#include <xview_private/fchsr_impl.h>

static void fc_calc_xs(Fc_private *private, Rect *exten_rect);
static int fc_calc_ys_top_down(Fc_private *private);
static int fc_calc_ys_bottom_up(Fc_private *private);
static int fc_recalc_ys(Fc_private *private, int top, Rect *exten_rect);
static int fc_calc_ys(Fc_private *private, int *y_anchor, Rect *exten_rect);
static int fc_calc_min_width(Fc_private *private);
static int fc_calc_min_height(Fc_private *private);
static int fc_calc_default_width(Fc_private *private, int min_width);
static int fc_calc_default_height(Fc_private *private, int min_height);

Pkg_private void file_chooser_position_objects(Fc_private *private);
Pkg_private void file_chooser_calc_min_size(Fc_private *private, int *width, int *height);
Pkg_private void file_chooser_calc_default_size(Fc_private *private, int min_width, int min_height, int *width, int *height);

#endif
