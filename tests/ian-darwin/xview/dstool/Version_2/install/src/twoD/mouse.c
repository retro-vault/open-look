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
/*
 * mouse.c
 *
 * Routines which define action of button clicks in twoD windows
 *
 *
 * Standard key translations are:
 *    LEFT   <==> SELECT
 *    MIDDLE <==> ADJUST
 *    RIGHT  <==> MENU
 */
#include <stdio.h>

#include <constants.h>
#include <pm.h>


int point_to_ic(int win, double absc, double ordin);

int mouse_select(int window_number, double cursor_absc, double cursor_ordin)
{
  pm(PUT, "Flow.Direction", FORWARD, 0);              /* set direction to FORWARDS */
  selected_read_window();
  point_to_ic(window_number, cursor_absc, cursor_ordin); /* set new initial conditions */
  sel_data_refresh();
}


int mouse_shift_select(int window_number, double cursor_absc, double cursor_ordin)
{
  /* do nothing */
}


int mouse_control_select(int window_number, double cursor_absc, double cursor_ordin)
{
  /* do nothing */
}


int mouse_adjust(int window_number, double cursor_absc, double cursor_ordin)
{
  orbit_read_window();
  orbit_data_refresh();
  pm(EXEC, "Flow.Continue", 0);
  sel_data_refresh();
  cmd_data_refresh();
}


int mouse_shift_adjust(int window_number, double cursor_absc, double cursor_ordin)
{
  /* do nothing */
}


int mouse_control_adjust(int window_number, double cursor_absc, double cursor_ordin)
{
  int get_num_param();

  clear_win_data( get_num_param(window_number) );
}


int mouse_menu(int window_number, double cursor_absc, double cursor_ordin)
{
  pm(PUT, "Flow.Direction", BACKWARD, 0);               /* set direction backwards */
  selected_read_window();
  point_to_ic(window_number, cursor_absc, cursor_ordin);   /* change initial conditions */
  sel_data_refresh();
}


int mouse_shift_menu(int window_number, double cursor_absc, double cursor_ordin)
{
  /* do nothing */
}


int mouse_control_menu(int window_number, double cursor_absc, double cursor_ordin)
{
  /* do nothing */
}


