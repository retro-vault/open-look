/*
 * Declares the i18n interfaces used by the olwmslave application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

#ifndef i18n_DEFINED
#define i18n_DEFINED

#ifdef OW_I18N_L4
#include <widec.h>
#include <locale.h>
#include <wctype.h>
#include <xview/xv_i18n.h>

typedef struct {
        XFontSet        fs;
        XFontSetExtents	*fsx;
        char            *fsn;
} XFontSetInfo;

extern wchar_t  *mbstowcsdup();
extern wchar_t  *ctstowcsdup();
extern char     *ctstombsdup();
extern char     *wcstoctsdup();
#endif /* OW_I18N_L4 */

#endif /* i18n_DEFINED */


