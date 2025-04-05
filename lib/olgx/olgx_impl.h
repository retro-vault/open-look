/*
 *#ident "@(#)olgx_impl.h	1.20 93/06/28 SMI"
 */

/*
 * Copyright 1990 Sun Microsystems
 */

/*
 * OPEN LOOK object drawing package
 */

#ifndef OL_PRIVATE_DEFINED
#define OL_PRIVATE_DEFINED

#ifdef OW_I18N
/*
 * I18N_Portability: May need to change the following #include to
 * pickup the wchar_t and X11R5(-ish) Xlib functions definitions.
 */
#include <widec.h>
#include <X11/Xlib.h>
#if XlibSpecificationRelease != 5
#include <X11/XlibR5.h>
#endif /* XlibSpecificationRelease != 5 */

#endif
#include <olgx/olgx.h>

#define STRING_SIZE 128 /* max size of a glyph font string */

#define VARHEIGHT_BUTTON_CORNER_DIMEN 7

#define False 0
#define True 1

/*
 * OPEN LOOK constant definitions
 */

/*
 * Macro definitions
 */
#define VARIABLE_LENGTH_MACRO(start_pos, offset) \
  for (i = 0; i < num_add; i++)                  \
  {                                              \
    string[start_pos + i] = offset + add_ins[i]; \
  }

#ifdef OW_I18N
#define textfontset utextfont.fontset
#define textfont utextfont.fontstruct
#endif

typedef struct _per_disp_res_rec
{
  Display *dpy;
  int screen;
  GC_rec *gc_list_ptr;
  Pixmap busy_stipple;
  Pixmap grey_stipple;
  struct _per_disp_res_rec *next;
} per_disp_res_rec, *per_disp_res_ptr;

/*
 * Definitions used by the color calculation code
 */
#define XRGB 0xffff
#define MAXRGB 0xff
#define MAXH 360
#define MAXSV 1000

#define VMUL 12  /* brighten by 20% (12 = 1.2*10) */
#define SDIV 2   /* unsaturate by 50% (divide by 2) */
#define VMIN 400 /* minimum highlight brightness of 40% */

typedef struct
{
  int r,
      g,
      b;
} RGB;

typedef struct
{
  int h,
      s,
      v;
} HSV;

#endif /* !OL_PRIVATE_DEFINED */
