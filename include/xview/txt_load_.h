/*
 * Declares public XView interfaces, types, and attributes for txt
 * load.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(TXT_LOAD__H)
#define TXT_LOAD__H

#include <xview/pkg.h>
#include <xview/panel.h>
#include <xview_private/txt_impl.h>


Pkg_private int open_cmd_proc(Frame fc, CHAR *path, CHAR *file, Xv_opaque client_data);
Pkg_private int save_cmd_proc(Frame fc, CHAR *path, int exists);


static int do_load_proc(Textsw_folio folio, Event *ie);
static Panel_setting load_cmd_proc(Panel_item item, Event *event);
static Panel_setting load_cmd_proc_accel(Panel_item item, Event *event);
static void create_load_items(Panel panel, Textsw_view_handle view);


Pkg_private Panel textsw_create_load_panel(Frame frame, Textsw_view_handle view);

#endif
