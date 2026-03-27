/*
 * Declares private XView interfaces, types, and macros for ntfy node.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NTFY_NODE__H)
#define NTFY_NODE__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private NTFY_NODE *ntfy_alloc_node(void);
pkg_private void ntfy_replenish_nodes(void);
pkg_private void ntfy_free_node(register NTFY_NODE *node);

#endif
