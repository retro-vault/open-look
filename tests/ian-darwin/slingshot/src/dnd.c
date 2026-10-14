#include <xview/xview.h>
#include <xview/dragdrop.h>
/*
 *	(c) Copyright 1991 Sun Microsystems, Inc.  All rights reserved.
 *	See LEGAL_NOTICE file for terms and restrictions.
 */

#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)dnd.c 1.7 92/05/08";
#endif
#endif


#include <xview/win_input.h>
#include <xview/sel_pkg.h>
#include <sspkg/canshell.h>
#include <sspkg/rectobj.h>

static void fake_reply(Rectobj object, Event	* event);


#define CALLBACK_DROP_PROC(object, dnd_event)				\
	{								\
	  dnd_callback = (Proc_ptr) xv_get(object, RECTOBJ_DROP_PROC);	\
	  if(dnd_callback)						\
		(dnd_callback)(paint_window, (dnd_event), 		\
			canvas_shell, object);				\
	}


void
rectobj_process_drop_event(Xv_window paint_window, Event		* event, Canvas_shell canvas_shell, Rectobj rectobj)
{
	Proc_ptr	dnd_callback;
	static Rectobj	last;
	Event		synth_event;

	/*
	 * Call only on ACTION_DRAG_PREVIEW, ACTION_DRAG_COPY, 
	 * ACTION_DRAG_MOVE, and ACTION_DRAG_LOAD event-actions.
	 */

	if(last != rectobj) {
		synth_event = *event;
		event_set_action(&synth_event, ACTION_DRAG_PREVIEW);

		if(last) {
			event_id(&synth_event) = LOC_WINEXIT;
			CALLBACK_DROP_PROC(last, &synth_event)
			last = 0;
		}
	}

	if(!xv_get(rectobj, RECTOBJ_ACCEPTS_DROP)) {
		fake_reply(rectobj, event);
		last = 0;
		return;
	}

	if(event_action(event) == ACTION_DRAG_PREVIEW) {
		if(last != rectobj && event_id(event) == LOC_DRAG) {
			synth_event = *event;
			event_id(&synth_event) = LOC_WINENTER;
			CALLBACK_DROP_PROC(rectobj, &synth_event)
		} else
			CALLBACK_DROP_PROC(rectobj, event)

		last = rectobj;
		return;
	} else {
		/* 
		 * On other ACTION_DRAG_* events, reset this to 0.
		 * This means the application will not get a synthetic
		 * LOC_WINEXIT after a drop event.
		 */
		last = 0;
	}
	CALLBACK_DROP_PROC(rectobj, event)
	if(!dnd_callback)
		fake_reply(rectobj, event);
}


static void
fake_reply(Rectobj object, Event	* event)
{
	Selection_requestor sel_req;

	if(event_action(event) != ACTION_DRAG_PREVIEW) {
		/*
		 * The drop event didn't go to any object,
		 * go through the steps so that the holder doesn't
		 * hickup with an unnecessary timeout.
		 */
	        sel_req = xv_create(
			xv_get(object, RECTOBJ_CANVAS), SELECTION_REQUESTOR, 
			0);

		(void) dnd_decode_drop(sel_req, event);
		dnd_done(sel_req);

		xv_destroy(sel_req);
	}
}

