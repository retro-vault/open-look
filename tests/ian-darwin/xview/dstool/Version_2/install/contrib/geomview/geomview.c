#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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
/* geomview.c 
 * 
 * standard routines for geomview window
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <xview/xview.h>

#include <pm.h>
#include <ui_init.h>
#include <constants.h>
#include "geomview_cui.h"
#include "geomview.h"

geomview_window1_objects *geomview_ip = 0;

/* 
 * geomview_open()
 *
 * displays the geomview window, creating it if necessary.
 */
geomview_open(use_default,left,top,width,height)
int   use_default;    /* either SET_WIN_CONFIG  or  DEFAULT_WIN_CONFIG */
int   left;           /* position (on screen) of left edge of window.  */
int   top;            /* position (on screen) of top edge of window.   */
int   width;          /* width of window;  width <= 0 means use default width */
int   height;         /* height of window; height <= 0 means use default width */
{
  Rect  
    *rect;
  void
    geomview_field_manager(),geomview_install();
  if (geomview_ip == 0)
    {
      /* create the window, make the owner the dstool cmd panel */
      geomview_ip = geomview_window1_objects_initialize(0, cmd_ip->win);
      register_win_and_type("Geomview",geomview_ip->window1,POPUP_WINDOW);
      geomview_install();
      geomview_field_manager();
    }

  if(use_default == SET_WIN_CONFIG)
    {
      rect = (Rect *)calloc(1,sizeof(Rect));
      frame_get_rect(geomview_ip->window1,rect);
      rect->r_left = (short) left;
      rect->r_top = (short) top;
      if(width>0) rect->r_width = (short) width;
      if(height>0) rect->r_height = (short) height;
      frame_set_rect(geomview_ip->window1,rect);
      free(rect);
    }


  /* show the window */
  mark_window_open("Geomview");
  xv_set(geomview_ip->window1, FRAME_CMD_PUSHPIN_IN, TRUE, 0);
  xv_set(geomview_ip->window1, WIN_SHOW, TRUE, WIN_FRONT, 0);
}

int
geomview_close(void)
{
    mark_window_closed("Geomview");
    if(geomview_ip) {
	xv_set(geomview_ip->window1, FRAME_CMD_PUSHPIN_IN, FALSE, 0);
        xv_set(geomview_ip->window1, XV_SHOW, FALSE, 0);	
    }
}

/*
 * geomview_field_manager()
 *
 * manager for the custom panel items
 */
void
geomview_field_manager(void)
{
  int	i,status=0;
  char name[MAX_LEN_VARB_NAME];
  static int n_total=0;
  int	n_varb =  *((int *) pm( GET, "Model.Varb_Dim", 0 ));
  int	n_param = *((int *) pm( GET, "Model.Param_Dim", 0 ));
  int	n_funct = *((int *) pm( GET, "Model.Funct_Dim", 0 ));
  

/*  status = geomview_init(); */

  /* exit now if the window has not been created */
  if (geomview_ip == 0 || status != 0) return;   

  /* initialize data structure for the geomview window for the new dynamical system */
  status = geomview_init(); 

  /* destroy the custom items */
  if (n_total > 0)  
    {
      xv_destroy(geomview_ip->hor);
      xv_destroy(geomview_ip->ver);
      xv_destroy(geomview_ip->depth);
    }

  n_total = n_varb + n_param + n_funct; 
  
  geomview_ip->hor = geomview_window1_hor_create(geomview_ip, geomview_ip->controls1, n_total);
  geomview_ip->ver = geomview_window1_ver_create(geomview_ip, geomview_ip->controls1, n_total);
  geomview_ip->depth = geomview_window1_depth_create(geomview_ip, geomview_ip->controls1, n_total);

  for (i=0; i<n_varb; i++)
    {
      pm(GET, "Model.Varb_Names", i, name, 0);
      xv_set(geomview_ip->hor, PANEL_CHOICE_STRING, i, name, 0);
      xv_set(geomview_ip->ver, PANEL_CHOICE_STRING, i, name, 0);
      xv_set(geomview_ip->depth, PANEL_CHOICE_STRING, i, name, 0);
    }	
  for(i=n_varb;i<n_varb+n_param;i++) {
      pm( GET,"Model.Param_Names", i-n_varb , name , 0);
      xv_set(geomview_ip->hor,PANEL_CHOICE_STRING,i,name,0); 
      xv_set(geomview_ip->ver,PANEL_CHOICE_STRING,i,name,0);
      xv_set(geomview_ip->depth, PANEL_CHOICE_STRING, i, name, 0);
  }
  for(i=n_varb+n_param;i<n_total;i++) {
      pm( GET,"Model.Funct_Names" , i-n_varb-n_param , name , 0);
      xv_set(geomview_ip->hor,PANEL_CHOICE_STRING,i,name,0); 
      xv_set(geomview_ip->ver,PANEL_CHOICE_STRING,i,name,0);
      xv_set(geomview_ip->depth, PANEL_CHOICE_STRING, i, name, 0);
  }

  /* make window tall enough  */
  window_fit(geomview_ip->controls1);
  window_fit(geomview_ip->window1);

  /* write data into the fields */
  geomview_data_refresh();

  return;
}

int
  geomview_init()
{
    char  *dirname;
    int   get_cur_dir();
    int	n_varb = *((int *) pm( GET, "Model.Varb_Dim", 0 ));  /* get dimen of phase */ 

    dirname = (char *)malloc(SIZE_OF_DIR_PLUS_FNAME);

    get_cur_dir(dirname);	/* get defaults from system */


    pm(PUT, "Geomview.Window_Number",0, 0);
    
    pm(PUT, "Geomview.Hor",0, 0);
    pm(PUT, "Geomview.Active_Hor_Index",0, 0);
    pm(PUT, "Geomview.Active_Hor_Type",PHASE_SPACE_VARB, 0);


    if (n_varb > 1) {
      pm(PUT, "Geomview.Ver",1, 0);
      pm(PUT, "Geomview.Active_Ver_Index",1, 0);
    }
    else {
      pm(PUT, "Geomview.Ver",0, 0);
      pm(PUT, "Geomview.Active_Ver_Index",0, 0);
    }
    pm(PUT, "Geomview.Active_Ver_Type",PHASE_SPACE_VARB, 0);

    if (n_varb > 2 ) {
      pm(PUT, "Geomview.Depth",2, 0);
      pm(PUT, "Geomview.Active_Depth_Index",2, 0);
    }
    else if (n_varb == 2) {
      pm(PUT, "Geomview.Depth",1, 0);
      pm(PUT, "Geomview.Active_Depth_Index",1, 0);
    }
    else {
      pm(PUT, "Geomview.Depth",0, 0);
      pm(PUT, "Geomview.Active_Depth_Index",0, 0);
    }
    pm(PUT, "Geomview.Active_Depth_Type",PHASE_SPACE_VARB, 0);

    pm(PUT,"Geomview.Destination",GEOMVIEW,0);
    pm(PUT,"Geomview.Directory",dirname, 0);
    pm(PUT,"Geomview.Filename", "", 0);

    free(dirname);
    return(NO_ERROR);
}

/*
 * geomview_read_window()
 *
 * routine to read data from all items in the geomview window
 */
int
  geomview_read_window()
{
  int i,hor, ver, depth;
  char *strng;
  int	n_varb =  *((int *) pm( GET, "Model.Varb_Dim", 0 ));
  int	n_param = *((int *) pm( GET, "Model.Param_Dim", 0 ));
  int	n_funct = *((int *) pm( GET, "Model.Funct_Dim", 0 ));

  /* if no window, then nothing to read */
  if (geomview_ip == 0) return;

  /* read text fields */
  pm(PUT, "Geomview.Window_Number", (int) xv_get(geomview_ip->window_number, PANEL_VALUE), 0);


  pm(PUT,"Geomview.Directory", (char *) xv_get(geomview_ip->directory, PANEL_VALUE),0);	/* Destination Directory Name */
  pm(PUT,"Geomview.Filename", (char *) xv_get(geomview_ip->filename, PANEL_VALUE),0);	/* Destination Filename */

  /* read hor setting item */
  hor = (int) xv_get(geomview_ip->hor, PANEL_VALUE);
  pm(PUT, "Geomview.Hor",hor,0);
  if (hor < n_varb) {
      pm(PUT, "Geomview.Active_Hor_Type", PHASE_SPACE_VARB,0);
      pm(PUT, "Geomview.Active_Hor_Index", hor,0);
  }
  else if (hor < n_varb + n_param) {
      pm(PUT, "Geomview.Active_Hor_Type", PARAMETER_VARB,0);
      pm(PUT, "Geomview.Active_Hor_Index", hor - n_varb,0);
  } 
  else {
      pm(PUT, "Geomview.Active_Hor_Type", FUNCTION_VARB,0);
      pm(PUT, "Geomview.Active_Hor_Index", hor - n_varb - n_param,0);
  }

  /* read ver setting item */
  ver = (int) xv_get(geomview_ip->ver, PANEL_VALUE);
  pm(PUT, "Geomview.Ver",ver,0);
  if (ver < n_varb) {
      pm(PUT, "Geomview.Active_Ver_Type", PHASE_SPACE_VARB,0);
      pm(PUT, "Geomview.Active_Ver_Index", ver,0);
  }
  else if (ver < n_varb + n_param) {
      pm(PUT, "Geomview.Active_Ver_Type", PARAMETER_VARB,0);
      pm(PUT, "Geomview.Active_Ver_Index", ver - n_varb,0);
  } 
  else {
      pm(PUT, "Geomview.Active_Ver_Type", FUNCTION_VARB,0);
      pm(PUT, "Geomview.Active_Ver_Index", ver - n_varb - n_param,0);
  }

  /* read depth setting item */
  depth = (int) xv_get(geomview_ip->depth, PANEL_VALUE);
  pm(PUT, "Geomview.Depth",depth,0);
  if (depth < n_varb) {
      pm(PUT, "Geomview.Active_Depth_Type", PHASE_SPACE_VARB,0);
      pm(PUT, "Geomview.Active_Depth_Index", depth,0);
  }
  else if (depth < n_varb + n_param) {
      pm(PUT, "Geomview.Active_Depth_Type", PARAMETER_VARB,0);
      pm(PUT, "Geomview.Active_Depth_Index", depth - n_varb,0);
  } 
  else {
      pm(PUT, "Geomview.Active_Depth_Type", FUNCTION_VARB,0);
      pm(PUT, "Geomview.Active_Depth_Index", depth - n_varb - n_param,0);
  }


     pm(PUT, "Geomview.Destination", (int) xv_get(geomview_ip->destination, PANEL_VALUE),0);
}

/*
 * geomview_data_refresh()
 *
 * routine to refresh the data in all items in the geomview window
 */
int
  geomview_data_refresh()
{
  int format, i, n_varb;
  char		strng[SIZE_OF_DIR_PLUS_FNAME];

  /* if no window, then nothing to refresh */
  if (geomview_ip == 0) return;

  pm(GET, "Geomview.Directory", strng, 0);
  xv_set(geomview_ip->directory, PANEL_VALUE,strng, 0);
  pm(GET, "Geomview.Filename", strng, 0);
  xv_set(geomview_ip->filename, PANEL_VALUE,strng, 0);
  xv_set(geomview_ip->window_number, PANEL_VALUE,
	 *((int *) pm( GET, "Geomview.Window_Number", 0 )), 0);


  /* refresh setting items */
  xv_set(geomview_ip->hor, PANEL_VALUE, *((int *) pm( GET, "Geomview.Hor", 0 )), 0);
  xv_set(geomview_ip->ver, PANEL_VALUE, *((int *) pm( GET, "Geomview.Ver", 0 )), 0);
  xv_set(geomview_ip->depth, PANEL_VALUE, *((int *) pm( GET, "Geomview.Depth", 0 )), 0);
 
 xv_set(geomview_ip->destination, PANEL_VALUE, *((int *) pm( GET, "Geomview.Destination", 0 )), 0);

}

/*
 * geomview_handler()
 *
 * Menu handler for `panelmenu (Geomview...)'.
 */

Menu_item
  geomview_handler(item, op)
Menu_item item;
Menu_generate op;
{
  int locn[4];

  if (op == MENU_NOTIFY)
    {
      if (item != (Menu_item) 0)
	geomview_open(DEFAULT_WIN_CONFIG,0,0,0,0);
      else 
	{
	  pm(GET_LIST, "Win.Locn.Geomview", 0, 3, locn, 0);
	  if ((locn[0] != NO_LOCATION))
	    geomview_open(SET_WIN_CONFIG,locn[0],locn[1],locn[2],locn[3]);
	  else
	    geomview_open(DEFAULT_WIN_CONFIG,0,0,0,0);
	}
    }

  return item;
}
