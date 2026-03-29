/*
 * Declares the ds_popup interfaces used by the clock application.
 *
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * Sun design patents pending in the U.S. and foreign countries.
 *
 * Adapted to the CMake build system by Tomaz Stih
 *
 */

/*
 * Location ops 
 */
#define DS_POPUP_RIGHT 0	/* Place popup to right of baseframe */
#define DS_POPUP_LEFT 1		/* Place popup to left of baseframe */
#define DS_POPUP_ABOVE 2	/* Place popup above baseframe */
#define DS_POPUP_BELOW 3	/* Place popup below baseframe */
#define DS_POPUP_LOR 4		/* Place popup to right or left of baseframe */
#define DS_POPUP_AOB 5		/* Place popup above or below baseframe */

#include <xview/frame.h>

void ds_position_popup(Frame baseframe, Frame popup, int location_op);
