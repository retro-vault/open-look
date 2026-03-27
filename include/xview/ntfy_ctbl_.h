/*
 * Declares public XView interfaces, types, and attributes for ntfy
 * ctbl.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NTFY_CTBL__H)
#define NTFY_CTBL__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

void ntfy_add_to_table(NTFY_CLIENT *client, NTFY_CONDITION *condition, int type);
void ntfy_remove_from_table(NTFY_CLIENT *client, NTFY_CONDITION *condition);
pkg_private NTFY_ENUM ntfy_new_enum_conditions(NTFY_CNDTBL *cnd_list, NTFY_ENUM_FUNC enum_func, NTFY_ENUM_DATA context);
pkg_private NTFY_ENUM ntfy_new_paranoid_enum_conditions(NTFY_CNDTBL *cnd_list, NTFY_ENUM_FUNC enum_func, NTFY_ENUM_DATA context);
#ifdef notdef
void dump_table(int type);
#endif

#endif
