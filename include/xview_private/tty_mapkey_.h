/*
 * Declares private XView interfaces, types, and macros for tty mapkey.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TTY_MAPKEY__H)
#define TTY_MAPKEY__H

#include <xview/pkg.h>
#include <xview_private/term_impl.h>


Pkg_private void ttysw_readrc(struct ttysubwindow *ttysw);
void ttysw_doset(struct ttysubwindow *ttysw, char *var);
int ttysw_mapkey(Ttysw *ttysw, char *key, char *to, int output);
Pkg_private int ttysw_mapsetim(Ttysw_folio ttysw);
Pkg_private int ttysw_domap(Ttysw_folio ttysw, struct inputevent *ie);
int ttysw_strtokey(char *s);
Pkg_private void ttysw_display_capslock(struct ttysubwindow *ttysw);

static char *savestr(char *s);
static char *tdecode(register char *src, char *dst);
static char *str_index(char *domain, char *pat);
static void ttysw_add_caps(char *label, char *label_ptr);
static void ttysw_remove_caps(char *label, char *label_ptr);
static void ttysw_arrow_keys_to_string(unsigned xv_id, char *str);

#endif
