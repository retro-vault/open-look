#if !defined(OL_INIT__H)
#define OL_INIT__H

#include <olgx_impl.h>

Graphics_info *olgx_main_initialize(Display *dpy, int screen, unsigned int depth, int d_flag, XFontStruct *glyphfont_struct, XFontStruct *textfont_struct, unsigned long pixvals[], Pixmap stipple_pixmaps[]);
Graphics_info *olgx_initialize(Display *dpy, int screen, int d_flag, XFontStruct *glyphfont_struct, XFontStruct *textfont_struct, unsigned long pixvals[], Pixmap stipple_pixmaps[]);
#ifdef OW_I18N
Graphics_info *olgx_i18n_initialize(Display *dpy, int screen, unsigned int depth, int d_flag, XFontStruct *glyphfont_struct,
                                    XFontSet textfont_set, unsigned long pixvals[], Pixmap stipple_pixmaps[]);
#endif
void olgx_set_glyph_font(Graphics_info *info, XFontStruct *font_info, int flag);
#ifdef OW_I18N
static void olgx_real_set_text_font(Graphics_info *info, Olgx_font_or_fs ufont_info, int flag);
#endif
void olgx_set_text_font(Graphics_info *info, XFontStruct *font_info, int flag);
#ifdef OW_I18N
void olgx_set_text_fontset(Graphics_info *info, XFontSet font_set, int flag);
#endif
void olgx_error(char *string);
void olgx_set_single_color(Graphics_info *info, int index, unsigned long pixval, int flag);
unsigned long olgx_get_single_color(Graphics_info *info, int index);
void olgx_destroy(Graphics_info *info);
char *olgx_malloc(unsigned int nbytes);
GC_rec *olgx_get_gcrec(per_disp_res_ptr perdispl_res_ptr, Drawable drawable, int depth, unsigned long valuemask, XGCValues *values);
int gc_matches(GC_rec *GCrec, unsigned long valuemask, XGCValues *values);
GC_rec *olgx_gcrec_available(per_disp_res_ptr perdispl_res_ptr, unsigned long valuemask, XGCValues *values);
Pixmap olgx_get_grey_stipple(per_disp_res_ptr perdispl_res_ptr);
per_disp_res_ptr olgx_get_perdisplay_list(Display *dpy, int screen);
void olgx_destroy_gcrec(per_disp_res_ptr perdisp_res_ptr, GC_rec *gcrec);
GC_rec *olgx_set_color_smart(Graphics_info *info, per_disp_res_ptr perdispl_res_ptr, GC_rec *gcrec, int fg_flag, unsigned long pixval, int flag);
void olgx_total_gcs(Display *dpy, int screen);
void olgx_initialise_gcrec(Graphics_info *info, short index);
int olgx_cmp_fonts(XFontStruct *font_info1, XFontStruct *font_info2);
#ifdef OW_I18N
int olgx_cmp_fontsets(XFontSet *fontset_info1, XFontSet *fontset_info2);
#endif

#endif
