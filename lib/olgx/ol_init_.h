/*
 * ol_init_.h: declarations and shared types for the ol_init_ module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#if !defined(OL_INIT__H)
#define OL_INIT__H

#include <olgx_impl.h>

/* olgx_main_initialize: Initialize main. */
Graphics_info *olgx_main_initialize(Display *dpy, int screen, unsigned int depth, int d_flag, XFontStruct *glyphfont_struct, XFontStruct *textfont_struct, unsigned long pixvals[], Pixmap stipple_pixmaps[]);
/* olgx_initialize: Initialize module state. */
Graphics_info *olgx_initialize(Display *dpy, int screen, int d_flag, XFontStruct *glyphfont_struct, XFontStruct *textfont_struct, unsigned long pixvals[], Pixmap stipple_pixmaps[]);
#ifdef OW_I18N
/* olgx_i18n_initialize: Initialize i 18 n. */
Graphics_info *olgx_i18n_initialize(Display *dpy, int screen, unsigned int depth, int d_flag, XFontStruct *glyphfont_struct,
                                    XFontSet textfont_set, unsigned long pixvals[], Pixmap stipple_pixmaps[]);
#endif
/* olgx_set_glyph_font: Set glyph font. */
void olgx_set_glyph_font(Graphics_info *info, XFontStruct *font_info, int flag);
#ifdef OW_I18N
/* olgx_real_set_text_font: Set real text font. */
static void olgx_real_set_text_font(Graphics_info *info, Olgx_font_or_fs ufont_info, int flag);
#endif
/* olgx_set_text_font: Set text font. */
void olgx_set_text_font(Graphics_info *info, XFontStruct *font_info, int flag);
#ifdef OW_I18N
/* olgx_set_text_fontset: Set text fontset. */
void olgx_set_text_fontset(Graphics_info *info, XFontSet font_set, int flag);
#endif
/* olgx_error: Process error. */
void olgx_error(char *string);
/* olgx_set_single_color: Set single color. */
void olgx_set_single_color(Graphics_info *info, int index, unsigned long pixval, int flag);
/* olgx_get_single_color: Return single color. */
unsigned long olgx_get_single_color(Graphics_info *info, int index);
/* olgx_destroy: Destroy associated state. */
void olgx_destroy(Graphics_info *info);
/* olgx_malloc: Process malloc. */
char *olgx_malloc(unsigned int nbytes);
GC_rec *olgx_get_gcrec(per_disp_res_ptr perdispl_res_ptr, Drawable drawable, int depth, unsigned long valuemask, XGCValues *values);
/* gc_matches: Process gc matches. */
int gc_matches(GC_rec *GCrec, unsigned long valuemask, XGCValues *values);
GC_rec *olgx_gcrec_available(per_disp_res_ptr perdispl_res_ptr, unsigned long valuemask, XGCValues *values);
/* olgx_get_grey_stipple: Return grey stipple. */
Pixmap olgx_get_grey_stipple(per_disp_res_ptr perdispl_res_ptr);
per_disp_res_ptr olgx_get_perdisplay_list(Display *dpy, int screen);
/* olgx_destroy_gcrec: Destroy gcrec. */
void olgx_destroy_gcrec(per_disp_res_ptr perdisp_res_ptr, GC_rec *gcrec);
GC_rec *olgx_set_color_smart(Graphics_info *info, per_disp_res_ptr perdispl_res_ptr, GC_rec *gcrec, int fg_flag, unsigned long pixval, int flag);
/* olgx_total_gcs: Process total gcs. */
void olgx_total_gcs(Display *dpy, int screen);
/* olgx_initialise_gcrec: Process initialise gcrec. */
void olgx_initialise_gcrec(Graphics_info *info, short index);
/* olgx_cmp_fonts: Process cmp fonts. */
int olgx_cmp_fonts(XFontStruct *font_info1, XFontStruct *font_info2);
#ifdef OW_I18N
/* olgx_cmp_fontsets: Process cmp fontsets. */
int olgx_cmp_fontsets(XFontSet *fontset_info1, XFontSet *fontset_info2);
#endif

#endif
