/*
 * Declares internal interfaces used by the XView base module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_PARSE__H)
#define XV_PARSE__H

#include <xview/pkg.h>
#include <X11/Xresource.h>

Xv_private void xv_cmdline_scrunch(int *argc_ptr, char **argv, char **remove, int n);
Xv_public int xv_parse_cmdline(char *app_name, int *argc_ptr, char **argv_base, int scrunch);
Xv_private int xv_parse_one(char *app_name, register int, register char **argv);
Xv_private void xv_merge_cmdline(XrmDatabase *db);
Xv_private void xv_get_cmdline_str(char *str);
Xv_private void xv_get_cmdline_argv(char **argv, int *argc_ptr);

#endif
