/*
 * Implements the notice dstry routines used by the XView notice
 * module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/notice_dstry_.h>
#include <xview_private/notice_.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>
#include <xview_private/noticeimpl.h>
#include <xview/win_input.h>
#include <xview/notify.h>

Pkg_private int
notice_destroy_internal(notice_public, status)
    Xv_notice_struct	*notice_public;
    Destroy_status	status;
{
    Notice_info	*notice = NOTICE_PRIVATE(notice_public);


    if (status == DESTROY_CLEANUP)  {
	/*
	 * Free button structures
	 */
	if (notice->button_info)  {
            notice_free_button_structs(notice->button_info);
	    notice->button_info = (notice_buttons_handle)NULL;
	}

	if (notice->msg_info)  {
            notice_free_msg_structs(notice->msg_info);
	    notice->msg_info = (notice_msgs_handle)NULL;
	}

	/*
	 * Destroy subframe
	 */
	if (notice->sub_frame)  {
	    xv_set(notice->sub_frame, 
		XV_KEY_DATA, (Attr_attribute)notice_context_key, NULL, 
		NULL);
            xv_destroy_safe(notice->sub_frame);
	    notice->sub_frame = (Frame)NULL;
	}

	/*
	 * Destroy window used for screen locking notice
	 */
        if (notice->fullscreen_window)  {
	    xv_destroy(notice->fullscreen_window);
	    notice->fullscreen_window = (Xv_window)NULL;
        }

	/*
	 * Free list of busy frames
	 */
	if (notice->busy_frames)  {
	    free(notice->busy_frames);
	    notice->busy_frames = (Frame *)NULL;
	}

	/*
	 * Free notice struct
	 */
        free((char *)notice);
        notice_public->private_data = (Xv_opaque)NULL;
    }

    return(XV_OK);
}
