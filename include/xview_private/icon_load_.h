/*
 * FILE *icon_open_header(char *from_file, char *error_msg,
 * Xv_icon_header_info *info);.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ICON_LOAD__H)
#define ICON_LOAD__H

#include <xview/pkg.h>
#include <xview/icon.h>
#include <xview/icon_load.h>

FILE *icon_open_header(char *from_file, char *error_msg, Xv_icon_header_info *info);
struct pixrect *icon_load_mpr(char *from_file, char *error_msg);
Server_image icon_load_svrim(char *from_file, char *error_msg);
int icon_init_from_pr(Icon icon_public, register struct pixrect *pr);
int icon_load(Icon icon_public, char *from_file, char *error_msg);

#endif
