/*
 * Declares internal interfaces used by the XView canvas module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(CNVS_CB__H)
#define CNVS_CB__H

#ifdef OW_I18N
#include <xview/pkg.h>
#include <xview_private/cnvs_impl.h>
#include <X11/Xresource.h>

Pkg_private void canvas_preedit_start(XIC ic, XPointer client_data, XPointer callback_data);
Pkg_private void canvas_preedit_draw(XIC ic, XPointer client_data, XPointer callback_data);
Xv_private void cache_text_state(XIMPreeditDrawCallbackStruct *pre_edit, XIMPreeditDrawCallbackStruct *pe_cache);
Pkg_private void canvas_preedit_done(XIC ic, XPointer client_data, XPointer callback_data);
Pkg_private void canvas_preedit_caret(XIC ic, XPointer client_data, XPointer callback_data);
Pkg_private Canvas_pew *canvas_create_pew(Frame frame);
Pkg_private Notify_value canvas_pew_destory(Notify_client client, Destroy_status status);
Pkg_private Notify_value canvas_pew_event_proc(Window parent_win, Event *event, Notify_arg arg, Notify_event_type type);
Pkg_private void canvas_pew_done(Frame frame);
#endif 

#endif
