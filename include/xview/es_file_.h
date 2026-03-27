/*
 * Declares public XView interfaces, types, and attributes for es file.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(ES_FILE__H)
#define ES_FILE__H

#include <xview/pkg.h>
#include <xview_private/es.h>

Pkg_private void es_file_append_error(char *error_buf, CHAR *file_name, Es_status status);
#ifdef OW_I18N
Pkg_private Es_handle es_file_create(CHAR *name_wc, int options, Es_status *status);
#else
Pkg_private Es_handle es_file_create(char *name, int options, Es_status *status);
#endif
caddr_t es_file_get(Es_handle esh, Es_attribute attribute, ...);
Pkg_private int es_file_copy_status(Es_handle esh, CHAR *to);
Pkg_private Es_handle es_file_make_backup(register Es_handle esh, char *backup_pattern, Es_status *status);

#endif
