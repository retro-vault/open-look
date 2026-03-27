/*
 * Declares private XView interfaces, types, and macros for xv path
 * util.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(XV_PATH_UTIL__H)
#define XV_PATH_UTIL__H

#include <xview/pkg.h>
#include <xview/frame.h>
 #include <sys/stat.h>

Pkg_private void xv_error_sprintf(Frame frame, int use_footer, char *format, ...);
Pkg_private char *xv_strcpy (char *dest, char *src);
Pkg_private void xv_dirname(char *path);
Pkg_private char *xv_dircat(char *path, char *dir);
Pkg_private int xv_isdir(char *path);
Pkg_private char *xv_dirpart(char *path);
Pkg_private char *xv_basepart(char *path);
Pkg_private int xv_file_exists(char *path);
Pkg_private int xv_stat(char *path, struct stat *stats);
Pkg_private int xv_access(char *path, int mode);
Pkg_private char *xv_expand_path(char *path);
char *xv_realpath(char *file_name, char *resolved_name);

#endif
