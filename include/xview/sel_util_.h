/*
 * Declares public XView interfaces, types, and attributes for sel
 * util.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_UTIL__H)
#define SEL_UTIL__H

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview_private/sel_impl.h>

Pkg_private struct timeval *xv_sel_cvt_xtime_to_timeval(Time XTime);
Pkg_private Time xv_sel_cvt_timeval_to_xtime(struct timeval *timeV);
Pkg_private Sel_owner_info *xv_sel_find_selection_data(Display *dpy, Atom selection, Window xid);
Pkg_private Sel_owner_info *xv_sel_set_selection_data(Display *dpy, Atom selection, Sel_owner_info *sel_owner);
Xv_private Time xv_sel_get_last_event_time(Display *dpy, Window win); 
int xv_sel_add_prop_notify_mask(Display *dpy, Window win, XWindowAttributes *winAttr);
Pkg_private Sel_atom_list *xv_sel_find_atom_list(Display *dpy, Window xid);
Pkg_private Sel_prop_list *xv_sel_get_prop_list(Display *dpy);
Pkg_private Atom xv_sel_get_property(Display *dpy);
Pkg_private void xv_sel_free_property(Display *dpy, Atom prop);
Pkg_private int xv_sel_predicate(Display *display, register XEvent *xevent, char *args);
Pkg_private int xv_sel_check_selnotify(Display *display, register XEvent *xevent, char *args);
Pkg_private char *xv_sel_atom_to_str(Display *dpy, Atom atom, XID xid);
Pkg_private Atom xv_sel_str_to_atom(Display *dpy, char *str, XID xid);
Pkg_private int xv_sel_handle_error(int errCode, Sel_req_info *sel, Sel_reply_info *replyInfo, Atom target);
Pkg_private int xv_sel_block_for_event(Display *display, XEvent *xevent, int seconds, int (*predicate)(), char *arg);
Pkg_private Sel_req_tbl *xv_sel_set_reply(Sel_reply_info *reply);
Pkg_private Sel_req_tbl *xv_sel_add_new_req(Sel_req_tbl *reqTbl, Sel_reply_info *reply);
Pkg_private Sel_reply_info *xv_sel_get_reply(XEvent *event);
Pkg_private Notify_value xv_sel_handle_sel_timeout(Notify_client client, int which);
int xv_sel_end_request(Sel_reply_info *reply);
Pkg_private int xv_sel_check_property_event(Display *display, XEvent *xevent, char *args);
Xv_private void xv_sel_set_compat_data(Display *dpy, Atom selection, Window xid, int clientType);
Pkg_private void xv_sel_free_compat_data(Display *dpy, Atom selection);
Pkg_private void xv_sel_send_old_pkg_sel_clear(Display *dpy, Atom selection, Window xid, Time time);
Xv_private int xv_seln_handle_req(Sel_cmpat_info *cmpatInfo, Display *dpy, Atom sel, Atom target, Atom prop, Window req, Time time);
Pkg_private Sel_cmpat_info *xv_sel_get_compat_data(Display *dpy);
Xv_private void xv_sel_send_old_owner_sel_clear(Display *dpy, Atom selection, Window xid, Time time);

#endif
