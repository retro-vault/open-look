/*
 * Implements the txt tool routines used by the XView textsw module.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */
#include <xview_private/txt_tool_.h>
#include <xview_private/attr_.h>
#include <xview_private/gettext_.h>
#include <xview_private/txt_event_.h>
#include <xview_private/txt_file_.h>
#include <xview_private/win_cntral_.h>
#include <xview_private/primal.h>
#include <xview/notice.h>
#include <xview/frame.h>
#include <xview/wmgr.h>
#include <fcntl.h>
#define _NOTIFY_MIN_SYMBOLS
#include <xview/notify.h>
#undef _NOTIFY_MIN_SYMBOLS

#include <xview/win_struct.h>

Pkg_private void
textsw_default_notify(abstract, attrs)
    Textsw          abstract;
    Attr_attribute *attrs;
{
    register Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    register Frame  frame = xv_get(abstract, WIN_FRAME);
    Xv_Notice	text_notice;

    for (; *attrs; attrs = attr_next(attrs)) {
	switch ((int)*attrs) {
	  case TEXTSW_ACTION_TOOL_CLOSE:
	  case TEXTSW_ACTION_TOOL_MGR:
	  case TEXTSW_ACTION_TOOL_DESTROY:
	  case TEXTSW_ACTION_TOOL_QUIT:{
		switch ((Textsw_action) (*attrs)) {
		  case TEXTSW_ACTION_TOOL_CLOSE:
		    if (!xv_get(frame, FRAME_CLOSED))
			xv_set(frame, FRAME_CLOSED, TRUE, 0);
		    break;
		  case TEXTSW_ACTION_TOOL_MGR:{
			(void) win_post_event(frame,
				      (Event *) attrs[1], NOTIFY_IMMEDIATE);
			break;
		    }
		  case TEXTSW_ACTION_TOOL_QUIT:
		    if (textsw_has_been_modified(abstract)) {
			int             result;

                        text_notice = (Xv_Notice)xv_get(frame, 
                                XV_KEY_DATA, (Attr_attribute)text_notice_key, 
				NULL);
                        if (!text_notice)  {
                            text_notice = xv_create(frame, NOTICE,
                                NOTICE_LOCK_SCREEN, FALSE,
			        NOTICE_BLOCK_THREAD, TRUE,
                                NOTICE_MESSAGE_STRINGS,
				XV_MSG("The text has been edited.\n\
\n\
You may discard edits now and quit, or cancel\n\
the request to Quit and go back and either save the\n\
contents or store the contents as a new file."),
                                NULL,
                                NOTICE_BUTTON_YES, 
				XV_MSG("Cancel, do NOT Quit"),
                                NOTICE_BUTTON, 
				XV_MSG("Discard edits, then Quit"), (Attr_attribute)123,
                                NOTICE_STATUS, &result,
                                XV_SHOW, TRUE,
                                NULL);

                            xv_set(frame, 
                                XV_KEY_DATA, (Attr_attribute)text_notice_key, text_notice,
                                NULL);
                        }
                        else  {
                            xv_set(text_notice, 
				NOTICE_LOCK_SCREEN, FALSE,
			        NOTICE_BLOCK_THREAD, TRUE,
                                NOTICE_MESSAGE_STRINGS,
				XV_MSG("The text has been edited.\n\
\n\
You may discard edits now and quit, or cancel\n\
the request to Quit and go back and either save the\n\
contents or store the contents as a new file."),
                                NULL,
                                NOTICE_BUTTON_YES, 
				XV_MSG("Cancel, do NOT Quit"),
                                NOTICE_BUTTON, 
				XV_MSG("Discard edits, then Quit"), (Attr_attribute)123,
				NOTICE_STATUS, &result,
                                XV_SHOW, TRUE, 
                                NULL);
                        }

			if ((result == ACTION_STOP) || (result == NOTICE_YES) || (result == NOTICE_FAILED)) {
			    break;
			} else {
			    (void) textsw_reset(abstract, 0, 0);
			    (void) textsw_reset(abstract, 0, 0);
			}
		    }
		    xv_destroy_safe(frame);
		    break;
		  case TEXTSW_ACTION_TOOL_DESTROY:
		    xv_set(frame, FRAME_NO_CONFIRM, TRUE, NULL);
		    xv_destroy_safe(frame);
		    break;
		}
		break;
	    }
	  default:
	    break;
	}
    }
}
