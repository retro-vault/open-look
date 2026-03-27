/*
 * Declares internal interfaces used by the XView panel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(P_IMCB__H)
#define P_IMCB__H

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

#ifdef OW_I18N
Xv_private void panel_text_start (XIC ic, XPointer client_data, XPointer callback_data)
Xv_private void panel_text_draw(XIC ic, XPointer client_data, XPointer callback_data);
Xv_private void panel_text_done(XIC ic, XPointer XPointer client_data, XPointer callback_data)
Xv_private void panel_preedit_display(Item_info *ip, XIMPreeditDrawCallbackStruct *preedit_changes, Bool full)
Xv_private XIMPreeditDrawCallbackStruct *panel_get_preedit(Item_info *ip)
Xv_private void panel_set_preedit(Item_info *ip, XIMPreeditDrawCallbackStruct *preedit);
#endif 

#endif
