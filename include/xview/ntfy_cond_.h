/*
 * Declares public XView interfaces, types, and attributes for ntfy
 * cond.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(NTFY_COND__H)
#define NTFY_COND__H

#include <xview/pkg.h>
#include <xview_private/ntfy.h>

pkg_private NTFY_CONDITION *ntfy_find_condition(NTFY_CONDITION *condition_list, NTFY_TYPE type, register NTFY_CONDITION **condition_latest, NTFY_DATA data, int use_data);
pkg_private NTFY_CONDITION *ntfy_new_condition(NTFY_CONDITION **condition_list, NTFY_TYPE type, NTFY_CONDITION **condition_latest, NTFY_DATA data, int use_data);
pkg_private void ntfy_unset_condition(NTFY_CLIENT **client_list, NTFY_CLIENT *client, NTFY_CONDITION *condition, NTFY_CLIENT **client_latest, NTFY_WHO who);
pkg_private void ntfy_remove_condition(NTFY_CLIENT *client, NTFY_CONDITION *condition, NTFY_WHO who);
pkg_private NTFY_ENUM ntfy_paranoid_enum_conditions(NTFY_CLIENT *clients, NTFY_ENUM_FUNC enum_func, NTFY_ENUM_DATA context);
pkg_private NTFY_ENUM ntfy_enum_conditions(NTFY_CLIENT *clients, NTFY_ENUM_FUNC enum_func, NTFY_ENUM_DATA context);
Notify_value notify_nop(void);
void ntfy_reset_paranoid(void);

#endif
