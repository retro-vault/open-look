/*
 * Declares internal interfaces used by the XView win module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(WIN_INPUT__H)
#define WIN_INPUT__H

#include <xview/pkg.h>
#include <xview/server.h>
#include <xview/win_input.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

Xv_private void win_xmask_to_im(register unsigned int xevent_mask, register Inputmask *im);
Xv_private unsigned int win_im_to_xmask(Xv_object window, register Inputmask *im);
void input_imnull(struct inputmask *im);
void input_imall(struct inputmask *im);
Xv_object input_readevent(Xv_object window, Event *event);
void win_refuse_kbd_focus(Xv_object window);
void win_release_event_lock(Xv_object window);
int win_set_kbd_focus(Xv_object window, XID xid);
XID win_get_kbd_focus(Xv_object window);
void win_set_no_focus(Xv_object window, int state);
Xv_object xv_input_readevent(Xv_object window, Event *event, int block, int type, Inputmask *im);
Xv_object xview_x_input_readevent(Display *display, register Event *event, Xv_object req_window, int block, int type, unsigned int xevent_mask, XEvent *rep);
Xv_private Notify_value xv_input_pending(Display *dpy, int fd);
Xv_private int win_handle_window_accel(Event *event);
Xv_private int win_handle_menu_accel(Event *event);
Xv_private void win_get_cmdline_option(Xv_object window, char *str, char *appl_cmdline);
Xv_private void win_set_wm_command_prop(Xv_object window, char **argv, char **appl_cmdline_argv, int appl_cmdline_argc);
Xv_private void win_event_to_proc_with_ptr(Xv_opaque window_public, Atom event_type, XID sender_id, int x, int y);
Bool check_lang_mode(Xv_server server, Display *display, Event *event);
Xv_private XID xv_get_softkey_xid(Xv_server server, Display *display);
Pkg_private int win_handle_compose(Event *event, XComposeStatus *c_status, int last);

#endif
