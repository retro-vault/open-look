/*
 * Declares private XView interfaces, types, and macros for pf text.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(PF_TEXT__H)
#define PF_TEXT__H

#include <xview/pkg.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/font.h>

Xv_public int xv_pf_text(struct pr_prpos prpos, int op, Pixfont *pf, char *str);
Xv_public struct pr_size xv_pf_textbatch(register struct pr_prpos *dst, int *lenp, Pixfont *pf, register char *str);
Xv_public int xv_pf_textbound(struct pr_subregion *bound, register int len, register Pixfont *pf, register char *str);
Xv_public struct pr_size xv_pf_textwidth(int len, register Xv_font pf, register char *str);
#ifdef OW_I18N
Xv_public struct pr_size xv_pf_textwidth_wc(int len, register Xv_font pf, register wchar_t *ws);
#endif 

#endif
