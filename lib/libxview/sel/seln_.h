/*
 * Declares internal interfaces used by the XView sel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SELN__H)
#define SELN__H

#include <xview/pkg.h>
#include <xview/selection.h>
#include <xview_private/seln_impl.h>

void selection_set(struct selection *sel, int (*sel_write)(), int (*sel_clear)(), Xv_object window);
void selection_get(int (*sel_read)(), Xv_object window);
void selection_clear(Xv_object window);
char *selection_filename(void);

#endif
