/*
 * Declares private XView interfaces, types, and macros for file list.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FILE_LIST__H)
#define FILE_LIST__H

#include <xview/pkg.h>
#include <xview/file_list.h>

Pkg_private int file_list_init(Xv_opaque owner, File_list_public *public, Attr_avlist avlist);
Pkg_private Xv_opaque file_list_set(File_list public, Attr_avlist avlist);
Pkg_private Xv_opaque file_list_get(File_list_public *public, int *status, Attr_attribute attr, va_list valist);
Pkg_private int file_list_destroy(File_list_public *public, Destroy_status status);
Xv_public int file_list_no_case_ascend_compare(File_list_row *row1, File_list_row *row2);
Xv_public int file_list_no_case_descend_compare(File_list_row *row1, File_list_row *row2);
Xv_public int file_list_case_ascend_compare(File_list_row *row1, File_list_row *row2);
Xv_public int file_list_case_descend_compare(File_list_row *row1, File_list_row *row2);

#endif
