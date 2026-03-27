/*
 * Declares public XView interfaces, types, and attributes for font.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FONT__H)
#define FONT__H

#include <xview/pkg.h>
#include <X11/Xresource.h>
#include <xview_private/font_impl.h>

struct font_return_attrs {
    char	*name;
    char	*orig_name;
    char	*family;
    char	*style;
    char	*foundry;
    char	*weight;
    char	*slant;
    char	*setwidthname;
    char	*addstylename;
    int		size;
    int		small_size;
    int		medium_size;
    int		large_size;
    int		extra_large_size;
    int		scale;
    Font_info	*resize_from_font;
    int		rescale_factor;
    int		free_name, free_family, free_style, 
		free_weight, free_slant, free_foundry, 
		free_setwidthname, free_addstylename;
    char	delim_used;
#ifdef OW_I18N
    int		    type;
    char            *locale;
    char            **names;
    short	    free_names;
    char            *specifier;
#endif /*OW_I18N*/ 
    char	*encoding;
    char	*registry;
    Font_locale_info	*linfo;
    unsigned	no_size:1;
    unsigned	no_style:1;
};
typedef struct font_return_attrs *Font_return_attrs;

Pkg_private int font_init(Xv_opaque parent_public,  Xv_font_struct *font_public,  Attr_avlist avlist);
Pkg_private int font_destroy_struct(Xv_font_struct *font_public,  Destroy_status status);
Xv_private Xv_font xv_find_olglyph_font(Xv_font font_public);
Xv_private char *xv_font_regular(void);
Xv_private char *xv_font_monospace(void);
Xv_private char *xv_font_bold(void);
Xv_private char *xv_font_scale_cmdline(void);
Xv_private char *xv_font_regular_cmdline(void);
Xv_private char *xv_font_scale(void);
Pkg_private Xv_object font_find_font(Xv_opaque parent_public,  Xv_pkg *pkg,  Attr_avlist avlist);
Pkg_private int font_free_font_return_attr_strings(struct font_return_attrs *attrs);
Pkg_private int font_convert_style(Font_return_attrs return_attrs);
Pkg_private int font_convert_weightslant(Font_return_attrs return_attrs);
Pkg_private int font_fill_in_defaults(Font_return_attrs font_attrs);
Pkg_private int font_setup_pixfont(Xv_font_struct *font_public);
Pkg_private int font_check_var_height(int *variable_height_font, XFontStruct *x_font_info);
Pkg_private int font_check_overlapping(int *neg_left_bearing, XFontStruct *x_font_info);
Pkg_private int font_init_pixfont(Xv_font_struct *font_public);
#ifdef OW_I18N
char *get_attr_str_from_opened_names(char **names,  int attr_pos);
Pkg_private int font_scale_from_string(char *str);
#endif /* OW_I18N */
Xv_private Xv_Font xv_font_with_name(Xv_opaque server,  char *name);

#endif
