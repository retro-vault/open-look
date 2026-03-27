/*
 * Declares internal interfaces used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ES_CP_FILE__H)
#define ES_CP_FILE__H

#include <xview/pkg.h>
#include <xview_private/es.h>

Pkg_private int es_copy_file(CHAR *from, CHAR *to);
Pkg_private int es_copy_status(char *to, int fold, int *from_mode);
#ifdef OW_I18N
Pkg_private int es_copy_fd(CHAR *from_wc, CHAR *to_wc, int fold);
#else
Pkg_private int es_copy_fd(char *from, char *to, int fold);
#endif
#ifdef OW_I18N
Pkg_private int textsw_mbstowcs(CHAR *wstr, register char *str, int *n);
Pkg_private int es_mb_to_wc_fd(char *from, char *to, int fold, int *skipped);
#endif 

#endif
