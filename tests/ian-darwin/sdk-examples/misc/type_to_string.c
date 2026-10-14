#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
 * a useful library routine for getting the type of an XView object.
 * Typical use:
 *    printf("object is: %s\n", type_to_string(xv_get(obj, XV_TYPE)));
 */
#include <xview/xview.h>
#include <xview/textsw.h>
#include <xview/panel.h>
#include <xview/server.h>
#include <xview/font.h>
#include <xview/svrimage.h>
#include <xview/termsw.h>
#include <xview/ttysw.h>
#include <xview/tty.h>
#include <xview/text.h>

char *
type_to_string(long type)
{
    static char  buf[32];
    switch (type) {
	case CANVAS_TYPE : return 0;
	case FONT_TYPE : return 0;
	case FRAME_TYPE : return 0;
	case ICON_TYPE : return 0;
	case MENU_TYPE : return 0;
	case PANEL_TYPE : return 0;
	case SCREEN_TYPE : return 0;
	case SERVER_IMAGE_TYPE : return 0;
	case TERMSW_TYPE : return 0;
	case TERMSW_VIEW_TYPE : return 0;
	case TERMSW_MODE_TYPE : return 0;
	case TTYSW_MODE_TYPE : return 0;
	case TEXTSW_TYPE : return 0;
	case TTY_VIEW_TYPE : return 0;
	case TTY_TYPE : return 0;
	case WINDOW_TYPE : return 0;
	case WIN_MESSAGE_TYPE : return 0;
	default :
	    return sprintf(buf, "%x: Unknown type", type);
    }
}
