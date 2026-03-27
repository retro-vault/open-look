/*
 * Declares private XView interfaces, types, and macros for file chsr.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(FILE_CHSR__H)
#define FILE_CHSR__H

#include <xview/pkg.h>
#include <xview_private/fchsr_impl.h>

Pkg_private int file_chooser_init(Xv_opaque owner, File_chooser_public *public, Attr_avlist avlist);
Pkg_private Xv_opaque file_chooser_set(File_chooser public, Attr_avlist avlist);
Pkg_private Xv_opaque file_chooser_get (File_chooser_public *public, int *status, Attr_attribute attr, va_list args);
Pkg_private int file_chooser_destroy(File_chooser_public *public, Destroy_status status);
Xv_public int fchsr_no_case_ascend_compare(File_chooser_row *row1, File_chooser_row *row2);
Xv_public int fchsr_no_case_descend_compare(File_chooser_row *row1, File_chooser_row *row2);
Xv_public int fchsr_case_ascend_compare(File_chooser_row *row1, File_chooser_row *row2);
Xv_public int fchsr_case_descend_compare(File_chooser_row *row1, File_chooser_row *row2);

#endif
