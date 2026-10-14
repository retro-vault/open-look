/*  -------------------------------------------------------------------

This program is the property of:

                             Cornell University 
                        Center for Applied Mathematics 
                              Ithaca, NY 14853

and may be used, modified and distributed freely, subject to the 
following restrictions:

       Any product which incorporates source code from the dstool
       program or utilities, in whole or in part, is distributed
       with a copy of that source code, including this notice. You
       must give the recipients all the rights that you have with
       respect to the use of this software. Modifications of the
       software must carry prominent notices stating who changed
       the files and the date of any change.

DsTool is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of FITNESS 
FOR A PARTICULAR PURPOSE.  The software is provided as is without 
any obligation on the part of Cornell faculty, staff or students to 
assist in its use, correction, modification or enhancement.

  -----------------------------------------------------------------  */

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/xv_xrect.h>
#include <gdd.h>

#include <constants.h>
#include "mult_cui.h"


/*
 * Notify callback function for `forwards'.
 */
int mult_close(void);
int mult_read_window(void);

void
mult_forwards_notify(Panel_item item, Event		* event)
{
  mult_read_window();
  mult_data_refresh();
  selected_read_window();
  sel_data_refresh();
  orbit_read_window();
  orbit_data_refresh();
/*  prop_read_window();
  prop_data_refresh(); */

  pm(EXEC, "Mult.Forwards", 0);

  mult_data_refresh();
  cmd_data_refresh();
}


/*
 * Notify callback function for `continue'.
 */
void
mult_continue_notify(Panel_item item, Event		* event)
{
  mult_read_window();
  mult_data_refresh();
  selected_read_window();
  sel_data_refresh();
  orbit_read_window();
  orbit_data_refresh();
/*  prop_read_window();
  prop_data_refresh(); */

  pm(EXEC, "Mult.Continue", 0);

  mult_data_refresh();
  cmd_data_refresh();
}

/*
 * Notify callback function for `backwards'.
 */
void
mult_backwards_notify(Panel_item item, Event		* event)
{ 

  mult_read_window();
  mult_data_refresh();
  selected_read_window();
  sel_data_refresh();
  orbit_read_window();
  orbit_data_refresh();
/*  prop_read_window();
  prop_data_refresh(); */

  pm(EXEC, "Mult.Backwards", 0);

  mult_data_refresh();
  cmd_data_refresh();
}


/*
 * Notify callback function for `load_region'.
 */
void
mult_load_notify(Panel_item item, Event		* event)
{ 
  mult_read_window();
  mult_data_refresh();
  selected_read_window();
  sel_data_refresh();

  pm(EXEC, "Mult.Load", 0);

  mult_data_refresh();
}


/*
 * Notify callback function for `copy'.
 */
void
mult_copy_notify(Panel_item item, Event		* event)
{ 

        mult_read_window();
	mult_data_refresh();
  
	pm(EXEC, "Mult.Copy", 0);
}


/*
 * Notify callback function for setting type panel item.
 */
int
mult_settings_notify(Panel_item item, int value, Event		* event)
{
        mult_read_window();
	mult_data_refresh();
}

/*
 * Notify callback function for text field type panel item.
 */
Panel_setting
mult_text_notify(Panel_item item, Event		* event)
{
        mult_read_window();
	mult_data_refresh();

	return panel_text_notify(item, event);
}



/* callback for FRAME_DONE */
void
    mult_done_proc(frame)
Frame
    frame;
{
    mult_close();
}	




