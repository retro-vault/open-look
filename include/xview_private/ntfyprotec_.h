/*
 * Declares private XView interfaces, types, and macros for ntfyprotec.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NTFYPROTEC__H)
#define NTFYPROTEC__H

#include <xview/pkg.h> 
#include <xview_private/ntfy.h>

pkg_private void ntfy_end_critical(void);
pkg_private char *ntfy_malloc(u_int size);
pkg_private void ntfy_free_malloc(NTFY_DATA ptr);
pkg_private void ntfy_flush_tb_freed(void);

#endif
