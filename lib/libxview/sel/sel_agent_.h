/*
 * Declares internal interfaces used by the XView sel module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#if !defined(SEL_AGENT__H)
#define SEL_AGENT__H

#include <xview/pkg.h>
#include <xview/screen.h>
#include <xview/server.h>
#include <xview_private/seln_impl.h>

Xv_private int selection_init_agent(Xv_Server server, Xv_Screen screen);


Pkg_private Seln_result selection_agent_acquire(Xv_Server server, Seln_rank asked);
Pkg_private void selection_agent_get_holder(Xv_Server server, Seln_rank which, Seln_holder *holder);
Xv_private void selection_agent_clear(Xv_Server server, XSelectionClearEvent *clear_event);
Xv_private void selection_agent_selectionrequest(Xv_Server server, XSelectionRequestEvent *req_event);
Xv_private Seln_result seln_convert_request_to_property(Seln_request *buffer);
Pkg_private void seln_give_up_selection(Xv_Server server, Seln_rank rank);

static void selection_agent_process_functions(Xv_Server server, Seln_function_buffer *buffer);
static Seln_result selection_agent_process_request(Seln_attribute attr, register Seln_replier_data *context, int max_length);
static Seln_result seln_do_request_from_file(Seln_attribute attr, register Seln_replier_data *context, int fd, int max_length, Seln_agent_info *agent);
static Seln_result agent_do_yield(Seln_agent_info *agent, Seln_rank rank);
#ifdef OW_I18N
static Seln_result sel_get_selection(Xv_Server server, Display *display, Seln_agent_info *agent, Atom selection, Atom target);
static Seln_result sel_get_wcs(Xv_Server server, Atom selection, Seln_replier_data *replier_data, int length, Atom target);
static Seln_result seln_convert_request_to_ct_property(Seln_request *buffer);
#endif 
static Seln_result get_seln_int_request(Xv_Server server, Atom selection, Seln_replier_data *context, Atom target);
static Seln_result get_ascii_content(Xv_Server server, Atom selection, register Seln_replier_data *context, int length, Atom target);
static void selection_agent_do_timestamp(Xv_Server server, XSelectionRequestEvent *req_event, Seln_rank rank);
static void selection_agent_do_multiple(Xv_Server server, XSelectionRequestEvent *req_event);
static void selection_agent_do_target(Xv_Server server, XSelectionRequestEvent *req_event);
static void selection_agent_do_function(Xv_Server server, XSelectionRequestEvent *req_event);
static void selection_agent_do_request(Xv_Server server, XSelectionRequestEvent *req_event);
static void send_SelectionNotify(Display *display, Window requestor, Atom selection, Atom target, Atom property, Time time);
static void selection_agent_do_ascii_content(Xv_Server server, XSelectionRequestEvent *req_event);
static XSelectionEvent *ask_selection_owner(Display *display, Seln_agent_info *agent, Atom selection, Atom target, Atom property, Time time, int do_block);
static int is_blocking_over(Display *display, XEvent *xevent, char *args);
static void tvdiff(struct timeval *t1, struct timeval *t2, struct timeval *diff);
static void block(Display *display, XEvent *xevent, int seconds);
static int waitforReadableTimeout(Display *display, struct timeval *timeout);
static Seln_attribute convert_target_to_attr(Display *dpy, Seln_agent_info *agent, Atom target);
static Seln_attribute save_atom(Atom target, Display *dpy, Window win, char *atom_name, Seln_attribute attr, Atom *atom);
static Atom convert_attr_to_target(Display *dpy, Seln_agent_info *agent, Seln_attribute attr);
static Atom get_property_atom(Display *display, Seln_agent_info *agent);
static Atom get_atom(Display *dpy, Window win, char *prop_name, Seln_attribute attr, register Atom *current_value);

#endif
