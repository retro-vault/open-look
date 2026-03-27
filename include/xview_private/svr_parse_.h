/*
 * Declares private XView interfaces, types, and macros for svr parse.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SVR_PARSE__H)
#define SVR_PARSE__H

#include <xview/pkg.h>
#include <xview/server.h>
#include <xview_private/i18n_impl.h>
#include <X11/Xresource.h>


Xv_private int server_parse_keystr(Xv_server server_public, CHAR *keystr, KeySym *keysym, KeyCode *code, unsigned int *modifiers, unsigned int diamond_mask, char *qual_str);

#endif
